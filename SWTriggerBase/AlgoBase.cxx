#ifndef ALGOBASE_CXX
#define ALGOBASE_CXX

#include "AlgoBase.h"

namespace trigger {

  void AlgoBase::Configure()
  { this->_Configure_(); _configured = true; }

  const Result AlgoBase::Process(const WaveformArray_t& data)
  {
    if(!_configured) throw TriggerException("Must call Configure() before Process()!");
    _watch.Start();
    return this->_Process_(data);
    _time_profile  += _watch.WallTime();
    ++_process_count;
  }

  double AlgoBase::AverageProcessTime() const
  { return _time_profile / _process_count; }

}
#endif
