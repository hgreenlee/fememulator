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
    auto res = this->_Process_(data);
    res.pass_prescale = prescaleTrig();
    res.pass = res.pass_prescale | res.pass_algo;
    if ( res.pass_algo ) {
      // passes the algo. weight as 1
      res.prescale_factor = 1;
      res.weight = 1.0;
    }
    else {
      // weight up event if (!pass_algo & pass_prescale
      res.prescale_factor = _cfg.Get<int>("PrescaleFactor");
      res.weight = (float)res.prescale_factor;
    }
    _time_profile  += _watch.WallTime();
    ++_process_count;
    return res;
  }

  double AlgoBase::AverageProcessTime() const
  { return _time_profile / _process_count; }

  bool AlgoBase::prescaleTrig() {
    int prescale_factor = _cfg.Get<int>("PrescaleFactor");
    if ( prescale_factor<1 )
      throw TriggerException("PrescaleFactor in configuration must be >=1!");
    float border = 1.0/float(prescale_factor);
    std::mt19937::result_type seed = time(0);
    auto real_rand = std::bind(std::uniform_real_distribution<float>(0,1),std::mt19937(seed));
    if ( real_rand() < border ) return true;
    else return false;
  }
			      

}
#endif
