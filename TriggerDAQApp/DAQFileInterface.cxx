#ifndef _DAQFILE_INTERFACE_CXX__
#define _DAQFILE_INTERFACE_CXX__

#include "DAQFileInterface.h"
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

#include <TString.h>

namespace trigger {

  size_t _event;
  size_t _run;
  size_t _subrun;

  size_t _target_fem;
  size_t _target_ch_start;
  size_t _target_ch_count;

  double _trigger_time;
  double _waveform_time;

  LiteStorageManager::LiteStorageManager()
    : _event  (0)
    , _run    (0)
    , _subrun (0)
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

  void LiteStorageManager::SetTarget(size_t slot, size_t ch_start, size_t ch_count)
  {
    if(slot < 4 || slot > 19) throw TriggerException("Invalid FEM slot number requested");
    if(ch_start > 32) throw TriggerException("Invalid FEM channel number requested");
    if((32 - ch_start) < ch_count) throw TriggerException("# channel requested exceeding 32 (from start channel number)!");
    _target_fem = slot;
    _ch_start = ch_start;
    _ch_count = ch_count;
    _wf_v.resize(ch_count);
  }
  
  void LiteStorageManager::Reset()
  {
    if(_is) delete _is;
    _is = nullptr;
  }

  void LiteStorageManager::Initialize()
  {
    if(_current_input_index>=0)
      throw std::exception();
    if(_input_v.empty()) 
      throw std::exception();

    _current_input_index = 0;
    _eof = true;
    _event_ctr = 0;

    ::peek_at_next_event<ub_PMT_CardData_v6>(false);
    ::handle_missing_words<ub_PMT_CardData_v6>(true);
  }

  bool LiteStorageManager::ProcessEvent()
  {
    if(_current_input_index < 0){
      std::cerr << "Initialize first!" << std::endl;
      throw std::exception();
    }

    if(_eof){
      if(_current_input_index >= _input_v.size()) {
        std::cout << "Finished processing all files..." << std::endl;
        // if(!_outfile.empty() && _mgr.is_open()) _mgr.close();
        return false;
      }
      else {
        std::cout << "Opening a file: " << _input_v[_current_input_index].c_str() << std::endl;
        if(_is) { _is->close(); delete _is;}
        _is = new std::ifstream( _input_v[_current_input_index], std::ios::binary | std::ios::in);
        // _eventRecord = new ub_EventRecord;
        _eof = false;
        return ProcessEvent();
      }
    }

    //try{
    ::boost::archive::binary_iarchive ia(*_is);
    ub_EventRecord  eventRecord;
    ia >> eventRecord;
    ProcessRecord(eventRecord);
    //}catch(...) {
    //std::cerr << "Could not interpret data... skipping (possibly eof)" <<std::endl;
    //_eof = true;
    //_current_input_index +=1;
    //return ProcessEvent();
    //}
    ++_event_ctr;

    return true;
  }

  void LiteStorageManager::ProcessRecord(ub_EventRecord& eventRecord)
  {
    global_header_t const& globalHeader = eventRecord.getGlobalHeader();

    _trigger_time = -1;
    _trigger_bits = 0xffffffff;

    for(auto const& trig_map : eventRecord.getTRIGSEBMap()){
      
      auto const& trig_data = trig_map.second.getTriggerCardData();

      _trigger_time = (double)(trig_data.getFrame() * 1600. + trig_data.getSample() * (1./16.));
      _trigger_bits = (unsigned int)(trig_data.getTriggerData().trig_data_1);
      
      break;
    }

    // zero-fill waveforms
    for(auto& wf : _wf_v)
      for(auto& v : wf) v=0;

    for(auto const& seb : eventRecord.getPMTSEBMap()){
          
      //unsigned int crate = seb.first;
      auto const& crate_data = seb.second;
      
      if ( !crate_data.wasDissected() ) {
	std::cerr << "Warning crate data corrupted! Skipping." << std::endl;
	crate_data.dissectionException().what();
	continue;
      }

      bool first_card=true;
      for(auto const& card_data : crate_data.getCards()) {

	if( card_data.getModule() != _target_fem ) continue;
	
	if ( !card_data.wasDissected() ) {
	  std::cerr << "Warning card data corrupted! Skipping." << std::endl;
	  card_data.dissectionException().what();
	  continue;
	}

	unsigned int target_sample = 0;
	unsigned int target_frame  = 0;
	double min_dt = 1.e12;
	for(auto const& ch_data : card_data.getChannels()) {

	  if( ch_data.getChannelNumber() != _ch_start )
	    continue;

	  auto const& window_v = ch_data.getWindows();
	  for(auto const& window : window_v) {
	    auto const& window_header = window.header();
	    const ub_RawData& window_data = window.data();
	    
	    if(window_data.size()<1000) continue;
            
	    double ch_time = 1600. * RollOver(card_data.getFrame(),window_header.getFrame(),3) + window_header.getSample() * (1./64.);

	    double dt = (ch_time < _trigger_time ? _trigger_time - ch_time : ch_time - _trigger_time);

	    if(dt < min_dt) {
	      min_dt = dt;
	      target_frame  = window_header.getFrame();
	      target_sample = window_header.getSample();
	      _waveform_time = ch_time;
	    }
	  }

	  break;
	}
	if(!target_sample && !target_frame) throw TriggerException("Could not locate beamgates!");

	for(auto const& ch_data : card_data.getChannels()) {

	  if( ch_data.getChannelNumber() < _ch_start || ch_data.getChannelNumber() >= _ch_start + _ch_count )
	    continue;

	  auto const& window_v = ch_data.getWindows();
	  for(auto const& window : window_v) {
	    auto const& window_header = window.header();
	    const ub_RawData& window_data = window.data();
	    
	    if(window_data.size()<1000) continue;

	    if(window_header.getFrame() != target_frame || ch_data.getSample() != target_sample) continue;

	    auto& wf = _wf_v[ch_data.getChannelNumber()];

	    wf.resize(window_data.size());
	    for(size_t i=0; i<window_data.size(); ++i) { wf[i] = (0xfff & (window_data[i])); }
            
	  }
	}
	break;
      }	
    }
    _event = globalHeader.getEventNumber();
    _subrun = globalHeader.getSubrunNumber();
    _run = globalHeader.getRunNumber();
  }
  
}

#endif
