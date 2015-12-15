#include "FEMBeamTriggerAlgo.h"
#include "FEMBeamTriggerConstants.h"
#include <iostream>

namespace fememu {

  FEMBeamTriggerAlgo::FEMBeamTriggerAlgo()
  {}

  FEMBeamTriggerAlgo::FEMBeamTriggerAlgo(const FEMBeamTriggerConfig& cfg)
    : _cfg(cfg)
  {}

  const Waveform_t& FEMBeamTriggerAlgo::Multiplicity() const
  { return _chhit_sum; }

  const Waveform_t& FEMBeamTriggerAlgo::PHMAX() const
  { return _chdiff_sum; }

  const Waveform_t& FEMBeamTriggerAlgo::PhaseDiffDisc0(const size_t ch) const
  {
    if(ch >= _diff0_array.size()) throw EmulatorException("Requested non-existing channel info!");
    return _diff0_array[ch];
  }

  const Waveform_t& FEMBeamTriggerAlgo::PhaseDiffDisc3(const size_t ch) const
  {
    if(ch >= _diff3_array.size()) throw EmulatorException("Requested non-existing channel info!");
    return _diff3_array[ch];
  }

  const Waveform_t& FEMBeamTriggerAlgo::ChannelHitVector(const size_t ch) const
  {
    if(ch >= _chhit.size()) throw EmulatorException("Requested non-existing channel info!");
    return _chhit[ch];
  }

  const Waveform_t& FEMBeamTriggerAlgo::ChannelPHMAXVector(const size_t ch) const
  {
    if(ch >= _chdiff.size()) throw EmulatorException("Requested non-existing channel info!");
    return _chdiff[ch];
  }

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
    // Prepare output trigger object
    //
    std::vector<short> winstarts;
    size_t disc3_min_tick = 1e9;
    size_t disc3_max_tick = 0;
    if ( _cfg.fSetTriggerWindow ) {
      // define location of trigger window
      winstarts.push_back( _cfg.fTriggerWinStartTick );
      disc3_min_tick = _cfg.fTriggerWinStartTick;
      disc3_max_tick = _cfg.fWindowSize + disc3_min_tick;
    }
    else {
      // break up waveform shorto windows and calculate trigger vars for each window
      short nwindows = (wfmsize - 1 - _cfg.fFrontBuffer)/_cfg.fWindowSize;      
      for (short iwin=0; iwin<nwindows; iwin++) {
	short winstart = _cfg.fFrontBuffer + iwin*_cfg.fWindowSize;
	short winend   = _cfg.fFrontBuffer + (iwin+1)*_cfg.fWindowSize;
	if(!iwin) disc3_min_tick = winstart;
	if(iwin+1 == nwindows) disc3_max_tick = winend;
	//winid = iwin;
	if ( (unsigned short)winend>=wfmsize ) {
	  break;
	}
	winstarts.push_back( winstart );
      }
    }
    
    short nwindows = winstarts.size();
    if(debug()) std::cout << "[fememu::emulate] number of windows: " << nwindows << std::endl;

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

    _chdiff_sum.resize(wfmsize);
    _chhit_sum.resize(wfmsize);
    for(auto& v : _chdiff_sum) v = 0;
    for(auto& v : _chhit_sum ) v = 0;

    _diff0_array.resize(NChannels,Waveform_t(wfmsize,0));
    _diff3_array.resize(NChannels,Waveform_t(wfmsize,0));

    for (size_t ch=0; ch<NChannels; ch++) {
      
      auto const& wfm = chwfms[ch];

      auto& diff0(_diff0_array[ch]);
      auto& diff3(_diff3_array[ch]);

      for(auto& v: diff0) v = 0;
      for(auto& v: diff3) v = 0;
      
      for (short tick=_cfg.fDiscr0delay; tick<(short)wfm.size(); tick++)
	diff0[tick] = wfm[tick]-wfm[tick-_cfg.fDiscr0delay];
      for (short tick=_cfg.fDiscr3delay; tick<(short)wfm.size(); tick++)
	diff3[tick] = wfm[tick]-wfm[tick-_cfg.fDiscr3delay];

      if(debug()) std::cout << "[fememu::emulate] filled diffs for "  << ch << std::endl;

      // determine triggers and fill accumulators
      std::vector<short> ttrig0;
      std::vector<short> ttrig3;
    
      for (short tick=0; tick<(short)wfm.size(); tick++) {
	// discr0 must fire first: acts as pre-trigger. won't fire again until all discs are no longer active
	if ( diff0[tick]>=_cfg.fDiscr0threshold ) {
	  if ( ( ttrig0.size()==0 || ttrig0.back() + _cfg.fDiscr0precount < tick ) &&
	       ( ttrig3.size()==0 || ttrig3.back() + _cfg.fDiscr3deadtime < tick )
	       ) {
	    // form discr0 trigger
	    ttrig0.push_back( tick );
	  }
	} // end of if discr0 fires

	// discr3 fire
	if ( diff3[tick]>=_cfg.fDiscr3threshold ) {
	  // must be within discr0 prewindow and outside of past discr3 deadtime and inside beam spill window(s)
	  if ( ( ttrig0.size() >  0 && tick < _cfg.fDiscr0deadtime + ttrig0.back() ) &&
	       ( ttrig3.size() == 0 || ttrig3.back() + _cfg.fDiscr3deadtime < tick ) &&
	       ( tick >= disc3_min_tick && tick <= disc3_max_tick )
	       ) {
	    ttrig3.push_back( tick );
	    // // find maxdiff
	    short tmaxdiff = diff3[tick];
	    short tend1 = std::min( (short)(tick+_cfg.fDiscr3width), (short)diff3.size() );
	    for (short t=tick; t<tend1; t++) {
	      if ( tmaxdiff<diff3[t] )
	    	tmaxdiff = diff3[t];
	    }
	    if(debug())
	      std::cout << "[fememu::emulate] discr 3 fire: " << tmaxdiff << " " << diff3.at(tick) << std::endl;
	    // fill the accumulators
	    short tend = std::min( (short)(tick+_cfg.fDiscr3deadtime), (short)diff3.size() );
	    for (short t=tick; t<tend; t++) {
	      _chdiff[ch][ t ] = tmaxdiff;
	      _chhit[ch][ t ] = 1;
	    }
	  }
	}
      }//end of wfm loop for trigger and accumulators
      if(debug() && !ttrig3.empty())
	std::cout << "[fememu::emulate] found disc3 " << ttrig3.size() << " fires for "  << ch << std::endl;
    }//end of channel loop

    //
    // Fill sum vectors
    //
    for(size_t tick=0; tick<wfmsize; ++tick) {

      short phmax_sum = 0;
      short mult_sum  = 0;
      for(size_t ch=0; ch<NChannels; ++ch) {
	phmax_sum += _chdiff[ch][tick];
	mult_sum  += _chhit[ch][tick];
      }
      _chdiff_sum[tick] = phmax_sum;
      _chhit_sum[tick]  = mult_sum;
    }

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
	auto const& maxdiff_ = _chdiff_sum[tick];
	auto const& nhit_ = _chhit_sum[tick];

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

      if(debug()) 
	std::cout << "[fememu::emulate] WinID=" << iwin << " maxdiff=" << winmaxdiff << " maxhit=" << winmaxmulti << std::endl;
      if(info() && fire_time >= 0)
	std::cout << "[fememu::emulate] Trigger fired @ " << fire_time << " !" << std::endl;
    }
    
    return result;
  }
  
}
