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
	
	Waveform_t wf_subset(kMinBeamGateSize);
	for (size_t i=0; i < kMinBeamGateSize; i++)
	  wf_subset[i] = (unsigned short)wfm[i];

	chwfms.at( wfm.ChannelNumber() ) = wf_subset;

	nfilled++;
      }
      
      if(debug()) std::cout << " total waveforms: " << wfms.size() << ". nfilled: " << nfilled << std::endl;
      
      return ::trigger::fememu::FEMBeamTriggerAlgo::Process(0, chwfms );
    }
    
    const Result LLInterface::Emulate(const ::larlite::event_fifo& wfms )
    {
      
      WaveformArray_t chwfms(32);
      
      int nfilled = 0;
      for ( auto const& wfm : wfms ) {

	if (wfm.module_address() != _FEMSlot) continue;
	
	if ( wfm.channel_number()>kMaxPMTChannel || wfm.size()<kMinBeamGateSize ) continue;
	
	chwfms.at( wfm.channel_number() ) = wfm;
	nfilled++;
	
      }
      
      if(debug()) std::cout << " total waveforms: " << wfms.size() << ". nfilled: " << nfilled << std::endl;
      
      return ::trigger::fememu::FEMBeamTriggerAlgo::Process(0, chwfms );
    }
  }
}
#endif
