#include "FEMBeamTriggerAlgo.h"
#include "FEMBeamTriggerConstants.h"
#include <iostream>

namespace fememu {

  FEMBeamTriggerAlgo::FEMBeamTriggerAlgo()
  {}

  FEMBeamTriggerAlgo::FEMBeamTriggerAlgo(const FEMBeamTriggerConfig& cfg)
    : _cfg(cfg)
  {}

  const FEMBeamTriggerOutput FEMBeamTriggerAlgo::Emulate( const WaveformArray_t& chwfms )
  {
    if(debug()) std::cout << __PRETTY_FUNCTION__ << std::endl;
    
    /*
      Run basic check to make sure I did not get a crap:
      0) all waveform must be of equal length
      1) there should not be a channel above max ch (or just ignore it)
    */
    if(debug()) std::cout << "[fememu::emulate]" << std::endl;
      
    size_t wfmsize=0;
    for(auto const& wf : chwfms) {
      if(!wfmsize) {
	wfmsize = wf.size();
	if( wfmsize < kMinBeamGateSize ) throw EmulatorException("BeamGate size too short!");
      }
      else if(wfmsize != wf.size())
	throw EmulatorException("Waveform length unmatched!");
    }

    const size_t NChannels = chwfms.size();
    if(NChannels > (kMaxPMTChannel+1))
      throw EmulatorException("Found channel number larger than max specified (kMaxPMTChannel)");

    if ( wfmsize < (size_t)_cfg.fMinReadoutTicks ) {
      if(normal()) std::cout << "[fememu::emulate] Beam readout window size is too small! (" << wfmsize << " < " << _cfg.fMinReadoutTicks << ")" << std::endl;
      return FEMBeamTriggerOutput();
    }

    //
    // first calculate accumulators for each waveform
    //
    _chdiff.resize(NChannels);
    _chhit.resize(NChannels);
    for(auto& wfm : _chdiff) {
      wfm.resize(wfmsize);
      for(auto& v : wfm) v = 0;
    }
    for(auto& wfm : _chhit) {
      wfm.resize(wfmsize);
      for(auto& v : wfm) v = 0;
    }

    for (size_t ch=0; ch<NChannels; ch++) {
      
      auto const& wfm = chwfms[ch];

      _diff0.resize(wfmsize,0);
      _diff3.resize(wfmsize,0);
      for(auto& v: _diff0) v = 0;
      for(auto& v: _diff3) v = 0;
      
      for (short tick=_cfg.fDiscr0delay; tick<(short)wfm.size(); tick++)
	_diff0.at(tick) = wfm.at(tick)-wfm.at(tick-_cfg.fDiscr0delay);
      for (short tick=_cfg.fDiscr3delay; tick<(short)wfm.size(); tick++)
	_diff3.at(tick) = wfm.at(tick)-wfm.at(tick-_cfg.fDiscr3delay);

      if(debug()) std::cout << "[fememu::emulate] filled diffs for "  << ch << std::endl;

      // determine triggers and fill accumulators
      std::vector<short> ttrig0;
      std::vector<short> ttrig3;
    
      for (short tick=0; tick<(short)wfm.size(); tick++) {
	// discr0 must fire first: acts as pre-trigger. won't fire again until all discs are no longer active
	if ( _diff0.at(tick)>=_cfg.fDiscr0threshold ) {
	  if ( (ttrig0.size()==0 || ttrig0.at( ttrig0.size()-1 )+_cfg.fDiscr0precount<tick )
	       && ( ttrig3.size()==0 || ttrig3.at( ttrig3.size()-1 )+_cfg.fDiscr3deadtime < tick ) ) {
	    // form discr0 trigger
	    ttrig0.push_back( tick );
	  }
	} // end of if discr0 fires

	// discr3 fire
	if ( _diff3.at(tick)>=_cfg.fDiscr3threshold ) {
	  // must be within discr0 prewindow and outside of past discr3 deadtime
	  if ( ( ttrig0.size()>0 && tick-ttrig0.at( ttrig0.size()-1 ) < _cfg.fDiscr0deadtime )
	       && ( ttrig3.size()==0 || ttrig3.at( ttrig3.size()-1 ) + _cfg.fDiscr3deadtime < tick ) ) {
	    ttrig3.push_back( tick );
	    // // find maxdiff
	    short tmaxdiff = _diff3.at(tick);
	    short tend1 = std::min( (short)(tick+_cfg.fDiscr3width), (short)_diff3.size() );
	    for (short t=tick; t<tend1; t++) {
	      if ( tmaxdiff<_diff3.at(t) )
	    	tmaxdiff = _diff3.at(t);
	    }
	    if(info())
	      std::cout << "[fememu::emulate] discr 3 fire: " << tmaxdiff << " " << _diff3.at(tick) << std::endl;
	    // fill the accumulators
	    short tend = std::min( (short)(tick+_cfg.fDiscr3deadtime), (short)_diff3.size() );
	    for (short t=tick; t<tend; t++) {
	      _chdiff[ch].at( t ) = tmaxdiff;
	      _chhit[ch].at( t ) = 1;
	    }
	  }
	}
      }//end of wfm loop for trigger and accumulators
      if(info() && !ttrig3.empty())
	std::cout << "[fememu::emulate] found disc3 " << ttrig3.size() << " fires for "  << ch << std::endl;
    }//end of channel loop


        //
    // Prepare output trigger object
    //
    std::vector<short> winstarts;
    if ( _cfg.fSetTriggerWindow ) {
      // define location of trigger window
      winstarts.push_back( _cfg.fTriggerWinStartTick );
    }
    else {
      // break up waveform shorto windows and calculate trigger vars for each window
      short nwindows = (wfmsize - 1 - _cfg.fFrontBuffer)/_cfg.fWindowSize;      
      for (short iwin=0; iwin<nwindows; iwin++) {
	short winstart = _cfg.fFrontBuffer + iwin*_cfg.fWindowSize;
	short winend   = _cfg.fFrontBuffer + (iwin+1)*_cfg.fWindowSize;
	//winid = iwin;
	if ( (unsigned short)winend>=wfmsize ) {
	  break;
	}
	winstarts.push_back( winstart );
      }
    }
    
    short nwindows = winstarts.size();
    if(info()) std::cout << "[fememu::emulate] number of windows: " << nwindows << std::endl;

    // Prepare output (initialize in a way less change in mem seg)
    FEMBeamTriggerOutput result(nwindows);

    //
    // Fill trigger output
    //
    for (short iwin=0; iwin<nwindows; iwin++) {
      short winstart = winstarts.at(iwin);
      short winend   = winstart + _cfg.fWindowSize;
      //winid = iwin;
      if ( (size_t)winend>=wfmsize )
	continue;
      short winmaxmulti = 0;
      short winmaxdiff  = 0;
      int   fire_time   = -1;
      for (short tick=winstart; tick<winend; tick++) {
	short maxdiff_ = 0;
	short nhit_ = 0;
	for (size_t ch=0; ch<NChannels; ch++) {
	  maxdiff_ += _chdiff[ch].at(tick);
	  nhit_    += _chhit[ch].at(tick);
	}
	if ( winmaxdiff < maxdiff_ )
	  winmaxdiff = maxdiff_;
	if ( winmaxmulti < nhit_ )
	  winmaxmulti = nhit_;
	if(fire_time < 0 &&
	   maxdiff_  >= _cfg.fTriggerThresPHMAX &&
	   nhit_     >= _cfg.fTriggerThresMult)
	  fire_time = tick;

	if(debug()) std::cout << "    "
			      << "@ tick "   << tick     << "  "
			      << "@ window " << iwin     << "  "
			      << "mult = "   << nhit_    << "  "
			      << "phmax = "  << maxdiff_ << "  "
			      << std::endl;
      }

      // store results
      result.window_start_v[iwin] = winstart;
      result.window_end_v[iwin] = winend;
      result.vmaxdiff[iwin] = winmaxdiff;
      result.vmaxhit[iwin]  = winmaxmulti;
      result.fire_time_v[iwin] = fire_time;

      if(info()) std::cout << "[fememu::emulate] WinID=" << iwin << " maxdiff=" << winmaxdiff << " maxhit=" << winmaxmulti << std::endl;

      if(info() && fire_time>=0)
	if(normal()) std::cout << "[fememu::emulate] Trigger fired!" << std::endl;
    }
    
    return result;
  }
  
}
