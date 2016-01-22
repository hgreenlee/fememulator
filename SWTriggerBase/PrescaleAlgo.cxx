#include "PrescaleAlgo.h"
#include <iostream>

namespace trigger {
  
  PrescaleAlgo::PrescaleAlgo(const std::string name)
    : AlgoBase(name)
  {}
  
  const Result PrescaleAlgo::_Process_( unsigned int triggerbit, const WaveformArray_t& chwfms )
  {
    Result res;
    res.pass = true;
    // this class does nothing but pass.
    // if a prescale was set, the algobase will handle that
    return res;
  }
}
