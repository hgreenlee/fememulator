#ifndef ALGOBASE_CXX
#define ALGOBASE_CXX

#include "AlgoBase.h"

namespace trigger {

  void AlgoBase::Configure()
  { this->_Configure_(); _configured = true; }

  const Result AlgoBase::Process(const WaveformArray_t& data)
  {
    if(!_configured) throw TriggerException("Must call Configure() before Process()!");
    return this->_Process_(data);
  }

}
#endif
