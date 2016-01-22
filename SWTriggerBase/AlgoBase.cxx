#ifndef ALGOBASE_CXX
#define ALGOBASE_CXX

#include <random>
#include <ctime>
#include <functional>

#include "AlgoBase.h"

namespace trigger {

  std::map< std::string, AlgoFactory* > AlgoBase::_factories;

  void AlgoBase::Configure()
  { this->_Configure_(); _configured = true; _prescale_factor = _cfg.Get<int>("PrescaleFactor"); }

  const Result AlgoBase::Process(unsigned int triggerbit, const WaveformArray_t& data)
  {
    if(!_configured) throw TriggerException("Must call Configure() before Process()!");
    _watch.Start();
    auto res = this->_Process_(triggerbit, data);
    res.algo_instance_name = this->Name();
    res.pass_prescale = prescaleTrig();
    res.pass = res.pass_prescale && res.pass_algo; // pass is an AND of pre-scale and algo result
    res.prescale_weight = (float)_prescale_factor;

    _time_profile  += _watch.WallTime();
    ++_process_count;
    return res;
  }

  double AlgoBase::AverageProcessTime() const
  { return _time_profile / _process_count; }

  bool AlgoBase::prescaleTrig() {
    if ( _prescale_factor<1 )
      throw TriggerException("PrescaleFactor in configuration must be >=1!");
    float border = 1.0/float(_prescale_factor);
    std::mt19937::result_type seed = time(0);
    auto real_rand = std::bind(std::uniform_real_distribution<float>(0,1),std::mt19937(seed));
    if ( real_rand() < border ) return true;
    else return false;
  }
			      

}
#endif
