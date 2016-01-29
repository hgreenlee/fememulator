#include "PrescaleAlgo.h"
#include <iostream>

namespace trigger {

  static PrescaleAlgoFactory __global_PrescaleAlgoFactory__;  

  PrescaleAlgo::PrescaleAlgo(const std::string name)
    : AlgoBase(name)
  {}
  
  const Result PrescaleAlgo::_Process_( unsigned int triggerbit, const WaveformArray_t& chwfms )
  {
    Result res;
    res.pass_algo = true;
    // this class does nothing but pass.
    // if a prescale was set, the algobase will handle that
    return res;
  }
}
