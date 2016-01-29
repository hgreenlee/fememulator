#ifndef ALGOBASE_CXX
#define ALGOBASE_CXX

#include <random>
#include <ctime>
#include <iostream>
#include <functional>
#include "AlgoBase.h"

namespace trigger {

  AlgoBase::AlgoBase(const std::string name)
    : _cfg(name) , _time_profile(0) , _process_count(0) 
  { _cfg.Set<double>("PrescaleFactor",1.0); }

  void AlgoBase::Configure()
  { 
    this->_Configure_(); 
    _configured = true; 
    _prescale_factor = _cfg.Get<double>("PrescaleFactor"); 
    setPrescaleSeed( time(NULL) );
  }

  const Result AlgoBase::Process(unsigned int triggerbit, const WaveformArray_t& data)
  {
    if(!_configured) throw TriggerException("Must call Configure() before Process()!");
    _watch.Start();
    auto res = this->_Process_(triggerbit, data);
    res.algo_instance_name = this->Name();
    res.pass_prescale = prescaleTrig();
    res.pass = res.pass_prescale && res.pass_algo; // pass is an AND of pre-scale and algo result
    res.prescale_weight = 1.0/_prescale_factor;

    _time_profile  += _watch.WallTime();
    ++_process_count;
    return res;
  }

  double AlgoBase::AverageProcessTime() const
  { return _time_profile / _process_count; }

  bool AlgoBase::prescaleTrig() {
    if ( _prescale_factor>1 )
      throw TriggerException("PrescaleFactor in configuration must be a float between (0,1]!");
    double val = _randfunc();
    if ( val < _prescale_factor ) return true;
    else return false;
  }

  void AlgoBase::setPrescaleSeed( int seed ) {
    _randgen.seed( seed );
    _randfunc = std::bind(std::uniform_real_distribution<double>(0,1),_randgen);
  }
			      

}
#endif
