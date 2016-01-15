#ifndef __FEMEMULATOR_LARLITE_INTERFACE_CXX
#define __FEMEMULATOR_LARLITE_INTERFACE_CXX

#include "LArLiteInterface.h"
#include "FEMBeamTrigger/FEMBeamTriggerConstants.h"

namespace trigger {
  namespace fememu {
    
    const Result LLInterface::Emulate( const ::larlite::event_opdetwaveform& wfms )
    {
      
      WaveformArray_t chwfms(32);
      
      int nfilled = 0;
      for ( auto const& wfm : wfms ) {
	
	if ( wfm.ChannelNumber()>kMaxPMTChannel || wfm.size()<kMinBeamGateSize ) continue;

	Waveform_t copy_wfm(wfm.size(),0);
	for(size_t i=0; i<copy_wfm.size(); ++i) copy_wfm[i] = wfm[i];

	nfilled++;
      }
      
      if(debug()) std::cout << " total waveforms: " << wfms.size() << ". nfilled: " << nfilled << std::endl;
      
      return ::trigger::fememu::FEMBeamTriggerAlgo::Process( chwfms );
    }
    
    const Result LLInterface::Emulate(const ::larlite::event_fifo& wfms )
    {
      
      WaveformArray_t chwfms(32);
      
      int nfilled = 0;
      for ( auto const& wfm : wfms ) {
	
	if ( wfm.channel_number()>kMaxPMTChannel || wfm.size()<kMinBeamGateSize ) continue;
	
	chwfms.at( wfm.channel_number() ) = wfm;
	nfilled++;
	
      }
      
      if(debug()) std::cout << " total waveforms: " << wfms.size() << ". nfilled: " << nfilled << std::endl;
      
      return ::trigger::fememu::FEMBeamTriggerAlgo::Process( chwfms );
    }
  }
}
#endif
