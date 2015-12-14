#include "FEMBasicTriggerAlgo.h"
#include <iostream>

namespace fememu {

  TriggerOutput basicTrigger( const BasicTriggerConfig& config, const WaveformStore& chwfms ) {

    std::cout << __PRETTY_FUNCTION__ << std::endl;

    if (config.fVerbose )
      config.print();
    
    // first calculate accumulators for each waveform
    const int NChannels = chwfms.size();
    if (config.fVerbose)
      std::cout << "[fememu::baseicTrigger] NCHANNELS=" << NChannels << std::endl;

    std::vector<short> chdiff[NChannels];
    std::vector<short> chhit[NChannels];
    for (int ch=0; ch<NChannels; ch++) {
      
      const std::vector<short>& wfm = chwfms.get( ch );
      
      chdiff[ch].resize( wfm.size(), 0 );
      chhit[ch].resize( wfm.size(), 0 );
      
      // memory for diff vectors
      std::vector<short> diff0( (short)wfm.size(), 0 );
      std::vector<short> diff3( (short)wfm.size(), 0 );
      for (short tick=config.fDiscr0delay; tick<(short)wfm.size(); tick++) {
	diff0.at(tick) = wfm.at(tick)-wfm.at(tick-config.fDiscr0delay);
	//std::cout << diff0.at(tick) << std::endl;
      }
      for (short tick=config.fDiscr3delay; tick<(short)wfm.size(); tick++)
	diff3.at(tick) = wfm.at(tick)-wfm.at(tick-config.fDiscr3delay);

      // determine triggers and fill accumulators
      std::vector<short> ttrig0;
      std::vector<short> ttrig3;
    
      for (short tick=0; tick<(short)wfm.size(); tick++) {
	// discr0 must fire first: acts as pre-trigger. won't fire again until all discs are no longer active
	if ( diff0.at(tick)>=config.fDiscr0threshold ) {
	  if ( (ttrig0.size()==0 || ttrig0.at( ttrig0.size()-1 )+config.fDiscr0precount<tick )
	       && ( ttrig3.size()==0 || ttrig3.at( ttrig3.size()-1 )+config.fDiscr3deadtime < tick ) ) {
	    // form discr0 trigger
	    ttrig0.push_back( tick );
	  }
	} // end of if discr0 fires

	// discr3 fire
	if ( diff3.at(tick)>=config.fDiscr3threshold ) {
	  // must be within discr0 prewindow and outside of past discr3 deadtime
	  if ( ( ttrig0.size()>0 && tick-ttrig0.at( ttrig0.size()-1 ) < config.fDiscr0deadtime )
	       && ( ttrig3.size()==0 || ttrig3.at( ttrig3.size()-1 ) + config.fDiscr3deadtime < tick ) ) {
	    ttrig3.push_back( tick );
	    if (config.fVerbose)
	      std::cout << "[fememu::basicTrigger] discr3 fire at " << tick << ": " << diff3.at(tick) << std::endl;

	    // // find maxdiff
	    short tmaxdiff = diff3.at(tick);
	    short tend1 = std::min( (short)(tick+config.fDiscr3width), (short)diff3.size() );
	    for (short t=tick; t<tend1; t++) {
	      if ( tmaxdiff<diff3.at(t) )
	    	tmaxdiff = diff3.at(t);
	    }
	    // fill the accumulators
	    short tend = std::min( (short)(tick+config.fDiscr3deadtime), (short)diff3.size() );
	    for (short t=tick; t<tend; t++) {
	      chdiff[ch].at( t ) = tmaxdiff;
	      chhit[ch].at( t ) = 1;
	    }
	  }
	}
      }//end of wfm loop for trigger and accumulators
      if ( config.fVerbose )
	std::cout << "[fememu::basicTrigger] found " << ttrig3.size() << " trig fires for channel "  << ch  << std::endl;
    }//end of channel loop

    // break up waveform shorto windows and calculate trigger vars for each window
    // setup windows
    short wfmsize = (short)chwfms.get(0).size();
    std::vector<short> winstarts;
    if ( config.fSetBeamWindow )  {
      winstarts.push_back( config.fBeamWinStartTick );
    }
    else {
      if ( wfmsize < config.fMinReadoutTicks ) {
	std::cout << "Beam readout window size is too small! (" << wfmsize << " < " << config.fMinReadoutTicks << ")" << std::endl;
	return TriggerOutput();
      }

      short nwindows = (wfmsize-1-config.fFrontBuffer)/config.fBeamWinSize;
      for (short iwin=0; iwin<nwindows; iwin++) {
	short winstart = config.fFrontBuffer + iwin*config.fBeamWinSize;
	winstarts.push_back( winstart );
      }
    }

    if ( config.fVerbose )
      std::cout << "[fememu::basicTrigger] number of windows: " << winstarts.size() << std::endl;
    
    TriggerOutput out;
    out.vmaxdiff.clear();
    out.vmaxhit.clear();

    for (short iwin=0; iwin<(short)winstarts.size(); iwin++) {
      short winstart = winstarts.at(iwin);
      short winend   = winstart + config.fBeamWinSize;
      //winid = iwin;
      if ( winend>=wfmsize )
	continue;
      short winmaxmulti = 0;
      short winmaxdiff = 0;
      for (short tick=winstart; tick<winend; tick++) {
	short maxdiff_ = 0;
	short nhit_ = 0;
	for (short ch=0; ch<NChannels; ch++) {
	  maxdiff_ += chdiff[ch].at(tick);
	  nhit_    += chhit[ch].at(tick);
	}
	if ( winmaxdiff < maxdiff_ )
	  winmaxdiff = maxdiff_;
	if ( winmaxmulti < nhit_ )
	  winmaxmulti = nhit_;
      }


      if ( config.fVerbose )
	std::cout << "[fememu::basicTrigger] WinID=" << iwin << "  maxhit=" << winmaxmulti << " maxdiff=" << winmaxdiff << std::endl;

      // store for the window
      out.vmaxdiff.push_back( winmaxdiff );
      out.vmaxhit.push_back( winmaxmulti );
      
    }
    
    
    return out;
  }

}
