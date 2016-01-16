#ifndef _DAQFILE_INTERFACE_CXX__
#define _DAQFILE_INTERFACE_CXX__

#include "DAQFileInterface.h"
#include "UtilFunc.h"
#include "SWTriggerBase/TriggerException.h"
#include <iostream>
#include <iosfwd>
#include <memory>
#include <algorithm>
#include <iomanip>
#include <stdio.h>
#include <unistd.h>

#include "datatypes/raw_data_access.h"
#include "datatypes/uboone_data_utils.h"
//#include "datatypes/ub_EventRecord.h"

namespace trigger {

  size_t _event;
  size_t _run;
  size_t _subrun;

  size_t _target_fem;
  size_t _target_ch_start;
  size_t _target_ch_count;

  double _trigger_time;
  double _waveform_time;

  DAQFileInterface::DAQFileInterface()
    : _event  (0)
    , _run    (0)
    , _subrun (0)
    , _target_window_size(600)
    , _target_fem(4)
    , _target_ch_start(0)
    , _target_ch_count(32)
    , _trigger_time(-1)
    , _waveform_time(-1)
    , _current_input_index (-1)
    , _eventRecord(nullptr)
    , _outfile("")
    , _is(nullptr)
  {}

  void DAQFileInterface::SetTarget(size_t slot, size_t ch_start, size_t ch_count, size_t window_size)
  {
    if(slot < 4 || slot > 19) throw TriggerException("Invalid FEM slot number requested");
    if(ch_start > 32) throw TriggerException("Invalid FEM channel number requested");
    if((32 - ch_start) < ch_count) throw TriggerException("# channel requested exceeding 32 (from start channel number)!");
    _target_fem = slot;
    _target_ch_start = ch_start;
    _target_ch_count = ch_count;
    _target_window_size = window_size;
    _wf_v.resize(ch_count);
    for(auto& wf : _wf_v) wf.resize(window_size,0);
  }
  
  void DAQFileInterface::Reset()
  {
    if(_is) delete _is;
    _is = nullptr;
  }

  void DAQFileInterface::Initialize()
  {
    if(_current_input_index>=0)
      throw std::exception();
    if(_input_v.empty()) 
      throw std::exception();

    _current_input_index = 0;
    _event_ctr = 0;
    _event_ctr_per_file=0;
    ::peek_at_next_event<ub_PMT_CardData_v6>(false);
    ::handle_missing_words<ub_PMT_CardData_v6>(true);
    ::peek_at_next_event<ub_TPC_CardData_v6>(false);
    ::handle_missing_words<ub_TPC_CardData_v6>(true);
  }

  bool DAQFileInterface::ProcessEvent()
  {
    if(_current_input_index < 0){
      std::cerr << "Initialize first!" << std::endl;
      throw std::exception();
    }

    if(!_event_ctr_per_file) {
      if(_current_input_index >= _input_v.size()) {
        std::cout << "Finished processing all files..." << std::endl;
        // if(!_outfile.empty() && _mgr.is_open()) _mgr.close();
        return false;
      }
      else {

        if(_is) { _is->close(); delete _is;}
        std::cout << "Opening a file: " << _input_v[_current_input_index].c_str() << std::endl;
        _is = new std::ifstream( _input_v[_current_input_index], std::ios::binary | std::ios::in);
	uint16_t end_of_file_marker;
	uint32_t nevents;
	//eof marker is 16 bits long so go 16 bits from the end.
	_is->seekg( -1*sizeof(uint16_t) , std::ios::end);
	_is->read( (char*)&end_of_file_marker , sizeof(uint16_t));
	
	//make sure that it is the correct marker
	if(end_of_file_marker != 0xe0f0)
	  throw ::trigger::TriggerException("Incorrect EOF marker...");
	
	//need to go 48 bits from the end of the file (16 for eof marker and 32 for nevents
	_is->seekg( -3*sizeof(uint16_t) , std::ios::end); 
	//read in the 32 bits that should be the event count
	_is->read( (char*)&nevents, sizeof(uint32_t)); 
	
	//make sure that nevents is reasonable
	if (nevents>0 && nevents <1E9)
	  _event_ctr_per_file = nevents;
	else
	  throw ::trigger::TriggerException("Could not decode # events from file trailer...");
	_is->seekg(std::ios::beg);
      }
    }

    try{
      ::boost::archive::binary_iarchive ia(*_is);
      ub_EventRecord  eventRecord;
      ia >> eventRecord;
      ProcessRecord(eventRecord);
    } catch(...) {
      std::cerr << "Could not interpret data..." <<std::endl;
      throw ::trigger::TriggerException("Could not interpret data...");
    }
    ++_event_ctr;
    _event_ctr_per_file -= 1;
    if(!_event_ctr_per_file) ++_current_input_index;
    return true;
  }

  bool DAQFileInterface::ProcessRecord(ub_EventRecord& eventRecord)
  {
    global_header_t const& globalHeader = eventRecord.getGlobalHeader();

    if(eventRecord.getPMTSEBMap().size()<1 || eventRecord.getTRIGSEBMap().size()<1) {
      
      std::cerr << "\033[91m[ERROR]\033[00m No trigger and/or pmt data products found!" << std::endl;

      return false;
    }

    // zero-fill waveforms
    for(auto& wf : _wf_v)
      for(auto& v : wf) v=0;
       
    auto const& pmt_crate = eventRecord.getPMTSEBMap().begin()->second;
    auto const& trig_map  = eventRecord.getTRIGSEBMap();
    auto const& trig_header = trig_map.begin()->second.getTriggerHeader();
    auto const& trig_data = trig_map.begin()->second.getTriggerCardData().getTriggerData();

    size_t trig_sample_number = trig_header.get2MHzSampleNumber() * 32;
    trig_sample_number += trig_header.get16MHzRemainderNumber() * 4;
    trig_sample_number += trig_data.getPhase();

    auto const& pmt_card    = pmt_crate.getCards().at(0);
    //std::cout<<"Retrieving time-aligned waveforms... " << std::endl;
    pmt_card.fillBeamWindowWaveforms( trig_header.getFrame(), 
				      trig_sample_number,
				      _wf_v );

    _trigger_time = (double)(trig_header.getFrame() * 1600.);
    _trigger_time += ((double)trig_sample_number) * (1./64.e6);
    _trigger_bits = (unsigned int)(trig_data.trig_data_1);

    _event = globalHeader.getEventNumber();
    _subrun = globalHeader.getSubrunNumber();
    _run = globalHeader.getRunNumber();
  }
  
}

#endif
