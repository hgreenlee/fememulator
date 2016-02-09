#ifndef __FEMEMULATOR_LARLITE_INTERFACE_CXX
#define __FEMEMULATOR_LARLITE_INTERFACE_CXX

#include "LArLiteInterface.h"
#include "FEMBeamTrigger/FEMBeamTriggerConstants.h"

namespace trigger {
  namespace fememu {
    
    const Result LLInterface::Emulate( const ::larlite::event_opdetwaveform& wfms )
    {
      const size_t num_channel = 32;
      static WaveformArray_t target_wfms(num_channel,Waveform_t(kMinBeamGateSize,0));

      // Find target timing
      std::map<double,size_t> time_count_m;
      for ( auto const& wfm : wfms ) {
	
	if ( wfm.ChannelNumber()>=num_channel || wfm.size()<kMinBeamGateSize ) continue;

	auto iter = time_count_m.find(wfm.TimeStamp());
	if(iter == time_count_m.end()) time_count_m.emplace(wfm.TimeStamp(),1);
	else (*iter).second += 1;
      }

      // Find most popular timing
      double target_time = 0;
      size_t count_max=0;
      for( auto const& time_count : time_count_m) {

	if(time_count.second > count_max) {
	  count_max = time_count.second;
	  target_time = time_count.first;
	}

      }

      if(!count_max) throw std::exception();

      // Find target waveform index
      std::vector<size_t> target_index_v(num_channel,wfms.size()+1);
      std::vector<double> target_dt_v(num_channel,1e12);
      for(size_t wf_index=0; wf_index<wfms.size(); ++wf_index) {

	auto const& wfm = wfms[wf_index];

	if ( wfm.ChannelNumber()>=num_channel || wfm.size()<kMinBeamGateSize ) continue;

	double dt = wfm.TimeStamp() - target_time;

	if(dt<0) dt *= -1;

	auto& dt_record = target_dt_v[wfm.ChannelNumber()];

	if(dt < dt_record) {
	  dt_record = dt;
	  target_index_v[wfm.ChannelNumber()] = wf_index;
	}
	
      }

      size_t wfm_size=1e12;
      for( size_t ch=0; ch<num_channel; ++ch) {

	auto const& wfm_index = target_index_v[ch];

	if(wfm_index>wfms.size()) throw std::exception();

	if(wfm_size > wfms[wfm_index].size()) wfm_size = wfms[wfm_index].size();

      }

      for(size_t ch=0; ch<num_channel; ++ch) {

	auto const& wfm_index = target_index_v[ch];

	auto const& wfm = wfms[wfm_index];
	
	auto& target_wf = target_wfms[ch];

	target_wf.resize(wfm_size,0);

	for(size_t adc_index=0; adc_index<wfm_size; ++adc_index)
	  target_wf[adc_index] = wfm[adc_index];
      }
      
      return ::trigger::fememu::FEMBeamTriggerAlgo::Process(0, target_wfms );
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
