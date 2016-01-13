/**
 * \file DAQFileInterface.h
 *
 * \ingroup UBDAQApp
 * 
 * \brief Class def header for a class DAQFileInterface
 *
 * @author kterao
 */

/** \addtogroup UBDAQApp

    @{*/
#ifndef __DAQFILE_INTERFACE_H__
#define __DAQFILE_INTERFACE_H__

#ifndef __CINT__
#include <boost/archive/binary_iarchive.hpp>
#include <datatypes/ub_EventRecord.h>
#endif

#include "SWTriggerBase/SWTriggerTypes.h"
#include <fstream>
#include <vector>
#include <string>
#include "UtilFunc.h"
namespace trigger {
  /**
     \class DAQFileInterface
  */
  class DAQFileInterface {
    
  public:
    
    /// Default constructor
    LiteStorageManager();
    
    /// Default destructor
    ~LiteStorageManager(){ }

    void AddInputFile(const std::string fname) { _input_v.push_back(fname); }
    void SetTarget(size_t slot, size_t ch_start, size_t ch_count);
    void Reset();
    void Initialize();
    bool ProcessEvent();

    const WaveformArray_t& WaveformArray() const { return _wf_v; }
    double trigger_time  () const { return _trigger_time;  }
    double waveform_time () const { return _waveform_time; } 
    size_t event  () const { return _event;  }
    size_t run    () const { return _run;    }
    size_t subrun () const { return _subrun; }

  private:
#ifndef __CINT__
    void ProcessRecord(ub_EventRecord& eventRecord);
#endif 

    size_t _event;
    size_t _run;
    size_t _subrun;

    size_t _target_fem;
    size_t _target_ch_start;
    size_t _target_ch_count;

    double _trigger_time;
    double _waveform_time;

    WaveformArray_t _wf_v;
    bool _eof;
    std::vector<std::string> _input_v;
    int _current_input_index;
#ifndef __CINT__
    ub_EventRecord* _eventRecord;
#endif
    size_t _event_ctr;
    std::string _outfile;
    // ::larlite::storage_manager _mgr;
    std::ifstream* _is;

  };
}

#endif
/** @} */ // end of doxygen group 

