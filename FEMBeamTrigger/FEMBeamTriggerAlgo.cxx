#include "FEMBeamTriggerAlgo.h"
#include "FEMBeamTriggerConstants.h"
#include "FEMBeamTriggerOutput.h"
#include <iostream>

namespace uboonetrigger {

  FEMBeamTriggerAlgo::FEMBeamTriggerAlgo()
  {}
  
  FEMBeamTriggerAlgo::FEMBeamTriggerAlgo( std::unique_ptr<FEMBeamTriggerConfig> cfg ) 
  {
    _cfg = std::move(cfg);
  }

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

  std::unique_ptr<TriggerOutputBase> FEMBeamTriggerAlgo::Emulate( const WaveformArray_t& chwfms )
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

    if ( wfmsize < (size_t)cfg().fMinReadoutTicks ) {
      if(normal()) std::cout << "[fememu::emulate] Beam readout window size is too small! (" << wfmsize << " < " << cfg().fMinReadoutTicks << ")" << std::endl;
      std::unique_ptr<TriggerOutputBase> out = std::unique_ptr<FEMBeamTriggerOutput>(new FEMBeamTriggerOutput);
      return std::move(out);
    }
    
    //
    // Prepare output trigger object
    //
    size_t disc3_min_tick = cfg().fDiscr3WindowStart;
    size_t disc3_max_tick = cfg().fDiscr3WindowStart + cfg().fDiscr3WindowSize;
    if(disc3_max_tick > wfmsize) disc3_max_tick = wfmsize - 1;
    
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

      /*
      for (short tick=cfg().fDiscr0delay; tick<(short)wfm.size(); tick++)
	diff0[tick] = wfm[tick]-wfm[tick-cfg().fDiscr0delay];
      for (short tick=cfg().fDiscr3delay; tick<(short)wfm.size(); tick++)
	diff3[tick] = wfm[tick]-wfm[tick-cfg().fDiscr3delay];
      */

      for (short tick=0; (tick + cfg().fDiscr0delay)<(short)wfm.size(); tick++) {
	diff0[tick] = std::max( wfm[tick+cfg().fDiscr0delay] - wfm[tick], 0 );
	if(debug() && diff0[tick]>100) std::cout << "[fememu::emulate] diff0[" << tick << "]: " << diff0[tick] << std::endl;
      }
      for (short tick=0; (tick + cfg().fDiscr3delay)<(short)wfm.size(); tick++) {
	diff3[tick] = std::max( wfm[tick+cfg().fDiscr3delay] - wfm[tick], 0 );
	if(debug() && diff3[tick]>200) std::cout << "[fememu::emulate] diff3[" << tick << "]: " << diff3[tick] << std::endl;
      }
      if(debug()) std::cout << "[fememu::emulate] filled diffs for "  << ch << std::endl;

      // determine triggers and fill accumulators
      std::vector<short> tgth0;
      std::vector<short> ttrig0;
      std::vector<short> ttrig3;
    
      for (short tick=0; (tick+1)<(short)wfm.size(); tick++) {
	// discr0 must fire first: acts as pre-trigger. won't fire again until all discs are no longer active
	// In the firmware gth0 is different than edge0, which is what allows disc3
	if ( diff0[tick+1]>=cfg().fDiscr0threshold && diff0[tick]<cfg().fDiscr0threshold ) {
	  if ( ( tgth0.size()==0 || tgth0.back() + cfg().fDiscr0precount < tick+1 ) &&
	       ( ttrig3.size()==0 || ttrig3.back() + cfg().fDiscr3deadtime < tick+1 )
	       ) {
	    // form discr0 trigger
	    ttrig0.push_back( tick+1 );
	    if (debug()) std::cout << "[fememu::emulate] ttrig0 @ tick " << tick+1 << std::endl;
	  }
	  tgth0.push_back( tick+1 );
	  if (debug()) std::cout << "[fememu::emulate] tgth0 @ tick " << tick+1 << std::endl;
	} // end of if discr0 fires

	// There is no concept of a Discr0deatime in the firmware
	// discr3 fire
	// I am not satisfied with all the corrections in this block of code that deal with the fact
	// that the diff3 vector is only valid up to diff3.size()-cfg().fDiscr3width. Also, I don't
	// see why _cchhit shouldn't always have the same width pulse regardless of whether it was 
	// triggered near the end of a waveform or not.
	// I consider the following only valid if the beam window is far enough from the edge of the beam
	// gate, if the Discr3width is not too big, and if Discr0deadtime == 0.
	if ( diff3[tick+1]>=cfg().fDiscr3threshold && diff3[tick]<cfg().fDiscr3threshold ) {
	  // must be within discr0 prewindow and outside of past discr3 deadtime and inside beam spill window(s)
	  if ( ( !ttrig0.empty() && tick+1 < cfg().fDiscr0deadtime + ttrig0.back() ) &&
	       (  ttrig3.empty() || ttrig3.back() + cfg().fDiscr3deadtime < tick+1 ) &&
	       ( tick+1 >= disc3_min_tick && tick+1 <= disc3_max_tick )
	       ) {
	    ttrig3.push_back( tick+1 );
	    if (debug()) std::cout << "[fememu::emulate] ttrig3 @ tick " << tick+1 << std::endl;
	    // // find maxdiff
	    short tmaxdiff = diff3[ttrig0.back()];//diff3[tick+1];
	    //short tend1 = std::min( (short)(tick+1+cfg().fDiscr3width), (short)diff3.size() );
	    short tend1 = std::min( (short)(ttrig0.back()+cfg().fDiscr3width), (short)(diff3.size()-1));
	    for (short t=ttrig0.back(); t<tend1; t++) {
	      if(diff3[t]>=diff3[t-1]) {
		tmaxdiff = diff3[t];
	      }
	      _chdiff[ch][ t+1 ] = tmaxdiff;
	      if(t-ttrig0.back()>=3) {
		_chhit[ch][ t ] = 1;
	      }
	      //if ( tmaxdiff<diff3[t] )
	      //tmaxdiff = diff3[t];
	    }
	    if(info())
	      std::cout << "[fememu::emulate] tmax discr 3 fire: " << tmaxdiff << " " << diff3.at(tick+1) << " " << tick+1 << std::endl;
	    // fill the accumulators
	    /*
	    short tend = std::min( (short)(tick+1+cfg().fDiscr3deadtime), (short)diff3.size() );
	    for (short t=tick+1; t<tend; t++) {
	      _chdiff[ch][ t ] = tmaxdiff;
	      _chhit[ch][ t ] = 1;
	    }
	    */
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
    std::unique_ptr<FEMBeamTriggerOutput> result( new FEMBeamTriggerOutput );

    //
    // Fill trigger output
    //
    short winstart = cfg().fTriggerWindowStart;
    short winend   = cfg().fTriggerWindowStart + cfg().fTriggerWindowSize;

    if(winend >= wfmsize) winend = wfmsize - 1;

    short winmaxmulti = 0;
    short winmaxdiff  = 0;
    int   fire_time   = -1;
    for (short tick=winstart; tick<=winend; tick++) {
      auto const& maxdiff_ = _chdiff_sum[tick];
      auto const& nhit_ = _chhit_sum[tick];
      
      if ( winmaxdiff < maxdiff_ )
	winmaxdiff = maxdiff_;

      if ( winmaxmulti < nhit_ )
	winmaxmulti = nhit_;

      if(fire_time < 0 &&
	 maxdiff_  >= cfg().fTriggerThresPHMAX &&
	 nhit_     >= cfg().fTriggerThresMult )

	fire_time = tick; // fire time should be related to trig0 firing.
	
      
      if(debug()) std::cout << "    "
			    << "@ tick "   << tick     << "  "
			    << "mult = "   << nhit_    << "  "
			    << "phmax = "  << maxdiff_ << "  "
			    << std::endl;

      if(fire_time >= 0) break;
	
    }

    // store results
    result->window_start = winstart;
    result->window_end = winend;
    result->vmaxdiff = winmaxdiff;
    result->vmaxhit  = winmaxmulti;
    result->fire_time = fire_time;
    
    if(debug()) 
      std::cout << "[fememu::emulate] maxdiff=" << winmaxdiff << " maxhit=" << winmaxmulti << std::endl;
    if(info() && fire_time >= 0)
      std::cout << "[fememu::emulate] Trigger fired @ " << fire_time << " !" << std::endl;
    
    return std::move(result);
  }
  
}