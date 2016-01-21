#include "PrescaleAlgo.h"
#include <iostream>

namespace trigger {
  
  PrescaleAlgo::PrescaleAlgo(const std::string name)
    : AlgoBase(name)
    , _prescale(0)
    , _process_ctr(0)
  {DefineConfigParams();}
  
  void PrescaleAlgo::DefineConfigParams()
  {
    _cfg.Set("Prescale", 0);
  }
  
  void PrescaleAlgo::_Configure_()
  {
    _prescale = _cfg.Get<int>("Prescale");
  }
  
  const Result PrescaleAlgo::_Process_( const WaveformArray_t& chwfms )
  {
    ++_process_ctr;
    
    Result res;
    
    if(!(_process_ctr%_prescale)) res.pass = true;
    
    return res;
  }
}
