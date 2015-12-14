#include "FEMBeamTriggerAlgo.h"
#include "FEMBeamTriggerConstants.h"
#include <iostream>

namespace fememu {

  void FEMBeamTriggerAlgo::Reset(const FEMBeamTriggerConfig& cfg)
  {
    _cfg = cfg;
    _trigger.vmaxdiff.clear();
    _trigger.vmaxhit.clear();
    _trigger.fire_time_v.clear();
  }

  const FEMBeamTriggerOutput& FEMBeamTriggerAlgo::Emulate( const WaveformArray_t& chwfms )
  {

    //std::cout << __PRETTY_FUNCTION__ << std::endl;

    /*
      Run basic check to make sure I did not get a crap:
      0) all waveform must be of equal length
      1) there should not be a channel above max ch (or just ignore it)
    */
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

    // first calculate accumulators for each waveform

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

    for (int ch=0; ch<NChannels; ch++) {
      
      auto const& wfm = chwfms[ch];

      _diff0.resize(wfmsize,0);
      _diff3.resize(wfmsize,0);
      for(auto& v: _diff0) v = 0;
      for(auto& v: _diff3) v = 0;
      
      for (short tick=_cfg.fDiscr0delay; tick<(short)wfm.size(); tick++)
	_diff0.at(tick) = wfm.at(tick)-wfm.at(tick-_cfg.fDiscr0delay);
      for (short tick=_cfg.fDiscr3delay; tick<(short)wfm.size(); tick++)
	_diff3.at(tick) = wfm.at(tick)-wfm.at(tick-_cfg.fDiscr3delay);

      std::cout << "filled diffs for "  << ch << std::endl;

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
	    std::cout << "Find maxdiff" << std::endl;
	    short tmaxdiff = _diff3.at(tick);
	    short tend1 = std::min( (short)(tick+_cfg.fDiscr3width), (short)_diff3.size() );
	    for (short t=tick; t<tend1; t++) {
	      if ( tmaxdiff<_diff3.at(t) )
	    	tmaxdiff = _diff3.at(t);
	    }
	    // fill the accumulators
	    std::cout << "Fill accumulators" << std::endl;
	    short tend = std::min( (short)(tick+_cfg.fDiscr3deadtime), (short)_diff3.size() );
	    for (short t=tick; t<tend; t++) {
	      _chdiff[ch].at( t ) = tmaxdiff;
	      _chhit[ch].at( t ) = 1;
	    }
	  }
	}
      }//end of wfm loop for trigger and accumulators
      std::cout << "found trig fires for "  << ch << " (" << _chhit[ch].size() << ")" << std::endl;
    }//end of channel loop

    short nwindows = (wfmsize - 1 - _cfg.fFrontBuffer)/_cfg.fWindowSize;
    std::cout << "number of windows: " << nwindows << std::endl;

    // Prepare output (initialize in a way less change in mem seg)
    _trigger.vmaxdiff.resize(nwindows);
    _trigger.vmaxhit.resize(nwindows);
    _trigger.fire_time_v.clear();
    //_trigger.fire_time_v.resize(nwindows);
    for(size_t i=0; i<nwindows; ++i) {
      _trigger.vmaxdiff[i] = _trigger.vmaxhit[i] = 0;
      //_trigger.fire_time_v[i] = 0;
    }

    // break up waveform shorto windows and calculate trigger vars for each window
    std::cout << "size of wfm: " << wfmsize << std::endl;
    if ( wfmsize < _cfg.fMinReadoutTicks ) {
      std::cout << "Beam readout window size is too small! (" << wfmsize << " < " << _cfg.fMinReadoutTicks << ")" << std::endl;
      return _trigger;
    }

    for (short iwin=0; iwin<nwindows; iwin++) {
      short winstart = _cfg.fFrontBuffer + iwin*_cfg.fWindowSize;
      short winend   = _cfg.fFrontBuffer + (iwin+1)*_cfg.fWindowSize;
      //winid = iwin;
      if ( winend>=wfmsize )
	continue;
      short winmaxmulti = 0;
      short winmaxdiff = 0;
      for (short tick=winstart; tick<winend; tick++) {
	short maxdiff_ = 0;
	short nhit_ = 0;
	for (short ch=0; ch<NChannels; ch++) {
	  maxdiff_ += _chdiff[ch].at(tick);
	  nhit_    += _chhit[ch].at(tick);
	}
	if ( winmaxdiff < maxdiff_ )
	  winmaxdiff = maxdiff_;
	if ( winmaxmulti < nhit_ )
	  winmaxmulti = nhit_;
      }

      // store for the window
      _trigger.vmaxdiff[iwin] = winmaxdiff;
      _trigger.vmaxhit[iwin]  = winmaxmulti;
      //_trigger.fire_time_v[iwin] = 
    }

    return _trigger;
  }

}
