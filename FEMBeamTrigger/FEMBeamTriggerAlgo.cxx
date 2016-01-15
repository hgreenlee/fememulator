#include "FEMBeamTriggerAlgo.h"
#include "FEMBeamTriggerConstants.h"
#include <iostream>

namespace trigger {
  
  namespace fememu {
    
    FEMBeamTriggerAlgo::FEMBeamTriggerAlgo(const std::string name)
      : trigger::AlgoBase(name)
    {DefineConfigParams();}
    
    FEMBeamTriggerAlgo::FEMBeamTriggerAlgo(const FEMBeamTriggerConfig& cfg, const std::string name)
      : trigger::AlgoBase(name)
      , _fememu_cfg(cfg)
    {
      DefineConfigParams();
      Configure();
    }

    void FEMBeamTriggerAlgo::DefineConfigParams()
    {
      _cfg.Set("Verbosity",       (int)_fememu_cfg.fVerbosity       );

      _cfg.Set("Discr0threshold", (int)_fememu_cfg.fDiscr0threshold );
      _cfg.Set("Discr0delay",     (int)_fememu_cfg.fDiscr0delay     );
      _cfg.Set("Discr0deadtime",  (int)_fememu_cfg.fDiscr0deadtime  );
      _cfg.Set("Discr0precount",  (int)_fememu_cfg.fDiscr0precount  );

      _cfg.Set("Discr3threshold", (int)_fememu_cfg.fDiscr3threshold );
      _cfg.Set("Discr3delay",     (int)_fememu_cfg.fDiscr3delay     );
      _cfg.Set("Discr3deadtime",  (int)_fememu_cfg.fDiscr3deadtime  );
      _cfg.Set("Discr3width",     (int)_fememu_cfg.fDiscr3width     );

      _cfg.Set("Discr3WindowStart",  (int)_fememu_cfg.fDiscr3WindowStart  );
      _cfg.Set("Discr3WindowSize",   (int)_fememu_cfg.fDiscr3WindowSize   );

      _cfg.Set("TriggerWindowStart", (int)_fememu_cfg.fTriggerWindowStart );
      _cfg.Set("TriggerWindowSize",  (int)_fememu_cfg.fTriggerWindowSize  );

      _cfg.Set("MinReadoutTicks",    (int)_fememu_cfg.fMinReadoutTicks    );
      _cfg.Set("TriggerThresMult",   (int)_fememu_cfg.fTriggerThresMult   );
      _cfg.Set("TriggerThresPHMAX",  (int)_fememu_cfg.fTriggerThresPHMAX  );
    }
    
    void FEMBeamTriggerAlgo::_Configure_()
    {
      _fememu_cfg.fVerbosity = (MessageLevel_t)(_cfg.Get<int>("Verbosity"));

      _fememu_cfg.fDiscr0threshold = _cfg.Get<int>("Discr0threshold");
      _fememu_cfg.fDiscr0delay     = _cfg.Get<int>("Discr0delay"    );
      _fememu_cfg.fDiscr0deadtime  = _cfg.Get<int>("Discr0deadtime" );
      _fememu_cfg.fDiscr0precount  = _cfg.Get<int>("Discr0precount" );

      _fememu_cfg.fDiscr3threshold = _cfg.Get<int>("Discr3threshold");
      _fememu_cfg.fDiscr3delay     = _cfg.Get<int>("Discr3delay"    );
      _fememu_cfg.fDiscr3deadtime  = _cfg.Get<int>("Discr3deadtime" );
      _fememu_cfg.fDiscr3width     = _cfg.Get<int>("Discr3width"    );

      _fememu_cfg.fDiscr3WindowStart  = _cfg.Get<int>("Discr3WindowStart" );
      _fememu_cfg.fDiscr3WindowSize   = _cfg.Get<int>("Discr3WindowSize"  );

      _fememu_cfg.fTriggerWindowStart = _cfg.Get<int>("TriggerWindowStart");
      _fememu_cfg.fTriggerWindowSize  = _cfg.Get<int>("TriggerWindowSize" );

      _fememu_cfg.fMinReadoutTicks    = _cfg.Get<int>("MinReadoutTicks"   );
      _fememu_cfg.fTriggerThresMult   = _cfg.Get<int>("TriggerThresMult"  );
      _fememu_cfg.fTriggerThresPHMAX  = _cfg.Get<int>("TriggerThresPHMAX" );
    }
    
    const Waveform_t& FEMBeamTriggerAlgo::Multiplicity() const
    { return _chhit_sum; }
    
    const Waveform_t& FEMBeamTriggerAlgo::PHMAX() const
    { return _chdiff_sum; }
    
    const Waveform_t& FEMBeamTriggerAlgo::PhaseDiffDisc0(const size_t ch) const
    {
      if(ch >= _diff0_array.size()) throw TriggerException("Requested non-existing channel info!");
      return _diff0_array[ch];
    }
    
    const Waveform_t& FEMBeamTriggerAlgo::PhaseDiffDisc3(const size_t ch) const
    {
      if(ch >= _diff3_array.size()) throw TriggerException("Requested non-existing channel info!");
      return _diff3_array[ch];
    }
    
    const Waveform_t& FEMBeamTriggerAlgo::ChannelHitVector(const size_t ch) const
    {
      if(ch >= _chhit.size()) throw TriggerException("Requested non-existing channel info!");
      return _chhit[ch];
    }
    
    const Waveform_t& FEMBeamTriggerAlgo::ChannelPHMAXVector(const size_t ch) const
    {
      if(ch >= _chdiff.size()) throw TriggerException("Requested non-existing channel info!");
      return _chdiff[ch];
    }
    
    const ::trigger::Result FEMBeamTriggerAlgo::_Process_( const WaveformArray_t& chwfms )
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
	  if( wfmsize < kMinBeamGateSize ) throw TriggerException("BeamGate size too short!");
	}
	else if(wfmsize != wf.size())
	  throw TriggerException("Waveform length unmatched!");
      }
      
      const size_t NChannels = chwfms.size();
      if(NChannels > (kMaxPMTChannel+1))
	throw TriggerException("Found channel number larger than max specified (kMaxPMTChannel)");
      
      if ( wfmsize < (size_t)_fememu_cfg.fMinReadoutTicks ) {
	if(normal()) std::cout << "[fememu::emulate] Beam readout window size is too small! (" << wfmsize << " < " << _fememu_cfg.fMinReadoutTicks << ")" << std::endl;
	return Result();
      }
      
      //
      // Prepare output trigger object
      //
      size_t disc3_min_tick = _fememu_cfg.fDiscr3WindowStart;
      size_t disc3_max_tick = _fememu_cfg.fDiscr3WindowStart + _fememu_cfg.fDiscr3WindowSize;
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
	  for (short tick=_fememu_cfg.fDiscr0delay; tick<(short)wfm.size(); tick++)
	  diff0[tick] = wfm[tick]-wfm[tick-_fememu_cfg.fDiscr0delay];
	  for (short tick=_fememu_cfg.fDiscr3delay; tick<(short)wfm.size(); tick++)
	  diff3[tick] = wfm[tick]-wfm[tick-_fememu_cfg.fDiscr3delay];
	*/
	
	for (short tick=0; (tick + _fememu_cfg.fDiscr0delay)<(short)wfm.size(); tick++) {
	  diff0[tick] = (wfm[tick+_fememu_cfg.fDiscr0delay] > wfm[tick] ? wfm[tick+_fememu_cfg.fDiscr0delay] - wfm[tick] : 0 );
	  if(debug() && diff0[tick]>100) std::cout << "[fememu::emulate] diff0[" << tick << "]: " << diff0[tick] << std::endl;
	}
	for (short tick=0; (tick + _fememu_cfg.fDiscr3delay)<(short)wfm.size(); tick++) {
	  diff3[tick] = (wfm[tick+_fememu_cfg.fDiscr3delay] > wfm[tick] ? wfm[tick+_fememu_cfg.fDiscr3delay] - wfm[tick] : 0 );
	  if(debug() && diff3[tick]>200) std::cout << "[fememu::emulate] diff3[" << tick << "]: " << diff3[tick] << std::endl;
	}
	if(debug()) std::cout << "[fememu::emulate] filled diffs for "  << ch << std::endl;
	
	// determine triggers and fill accumulators
	std::vector<short> tgth0;
	std::vector<short> ttrig0;
	std::vector<short> ttrig3;
	ttrig0.reserve(100);
	ttrig3.reserve(100);
	tgth0.reserve(100);
	
	for (short tick=0; (tick+1)<(short)wfm.size(); tick++) {
	  // discr0 must fire first: acts as pre-trigger. won't fire again until all discs are no longer active
	  // In the firmware gth0 is different than edge0, which is what allows disc3
	  if ( diff0[tick+1]>=_fememu_cfg.fDiscr0threshold && diff0[tick]<_fememu_cfg.fDiscr0threshold ) {
	    if ( ( tgth0.size()==0 || tgth0.back() + _fememu_cfg.fDiscr0precount < tick+1 ) &&
		 ( ttrig3.size()==0 || ttrig3.back() + _fememu_cfg.fDiscr3deadtime < tick+1 )
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
	  // that the diff3 vector is only valid up to diff3.size()-_fememu_cfg.fDiscr3width. Also, I don't
	  // see why _cchhit shouldn't always have the same width pulse regardless of whether it was 
	  // triggered near the end of a waveform or not.
	  // I consider the following only valid if the beam window is far enough from the edge of the beam
	  // gate, if the Discr3width is not too big, and if Discr0deadtime == 0.
	  if ( diff3[tick+1]>=_fememu_cfg.fDiscr3threshold && diff3[tick]<_fememu_cfg.fDiscr3threshold ) {
	    // must be within discr0 prewindow and outside of past discr3 deadtime and inside beam spill window(s)
	    if ( ( !ttrig0.empty() && tick+1 < _fememu_cfg.fDiscr0deadtime + ttrig0.back() ) &&
		 (  ttrig3.empty() || ttrig3.back() + _fememu_cfg.fDiscr3deadtime < tick+1 ) &&
		 ( tick+1 >= disc3_min_tick && tick+1 <= disc3_max_tick )
		 ) {
	      ttrig3.push_back( tick+1 );
	      if (debug()) std::cout << "[fememu::emulate] ttrig3 @ tick " << tick+1 << std::endl;
	      // // find maxdiff
	      short tmaxdiff = diff3[ttrig0.back()];//diff3[tick+1];
	      //short tend1 = std::min( (short)(tick+1+_fememu_cfg.fDiscr3width), (short)diff3.size() );
	      short tend1 = std::min( (short)(ttrig0.back()+_fememu_cfg.fDiscr3width), (short)(diff3.size()-1));
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
		short tend = std::min( (short)(tick+1+_fememu_cfg.fDiscr3deadtime), (short)diff3.size() );
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
      ::trigger::Result result;
      
      //
      // Fill trigger output
      //
      short winstart = _fememu_cfg.fTriggerWindowStart;
      short winend   = _fememu_cfg.fTriggerWindowStart + _fememu_cfg.fTriggerWindowSize;
      
      if(winend >= wfmsize) winend = wfmsize - 1;
      
      short winmaxmulti = 0;
      short winmaxdiff  = 0;
      short maxmulti_attrig = 0;
      short maxdiff_attrig = 0;
      int   fire_time   = -1;
      for (short tick=winstart; tick<=winend; tick++) {
	auto const& maxdiff_ = _chdiff_sum[tick];
	auto const& nhit_ = _chhit_sum[tick];
	
	if ( winmaxdiff < maxdiff_ )
	  winmaxdiff = maxdiff_;
	
	if ( winmaxmulti < nhit_ )
	  winmaxmulti = nhit_;
	
	if(fire_time < 0 &&
	   maxdiff_  >= _fememu_cfg.fTriggerThresPHMAX &&
	   nhit_     >= _fememu_cfg.fTriggerThresMult ) {
	  
	  fire_time = tick; // fire time should be related to trig0 firing.
	  maxdiff_attrig = maxdiff_;
	  maxmulti_attrig = nhit_;
	}
	
	
	if(debug()) std::cout << "    "
			      << "@ tick "   << tick     << "  "
			      << "mult = "   << nhit_    << "  "
			      << "phmax = "  << maxdiff_ << "  "
			      << std::endl;
	
	if(fire_time >= 0) break;
	
      }
      
      // store results
      result.pass          = (fire_time >= 0);
      result.amplitude     = maxdiff_attrig;
      result.multiplicity  = maxmulti_attrig;
      result.time          = fire_time;
      
      if(debug()) 
	std::cout << "[fememu::emulate] maxdiff=" << winmaxdiff << " maxhit=" << winmaxmulti << std::endl;
      if(info() && fire_time >= 0)
	std::cout << "[fememu::emulate] Trigger fired @ " << fire_time << " !" << std::endl;
      
      return result;
    }
  }
}
