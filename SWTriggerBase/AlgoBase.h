/**
 * \file AlgoBase.h
 *
 * \ingroup SWTriggerBase
 * 
 * \brief Class def header for a class AlgoBase
 *
 * @author kazuhiro
 */

/** \addtogroup SWTriggerBase

    @{*/
#ifndef ALGOBASE_H
#define ALGOBASE_H

#include <iostream>
#include <random>
#include <ctime>

#include "ConfigHolder.h"
#include "Result.h"
#include "Watch.h"
namespace trigger {
  /**
     \class AlgoBase
     This is a base class for software algorithms. \n
     It assumes to make a trigger decision based on trigger::WaveformArray_t. \n
     It implements a mechanism to declare configuration parameters which are  \n
     meant to be filled by an owner with appropriate parameter values prior to \n
     an algorithm execution. A child class should implement a variable declaration \n
     in the configuration object via ConfigHolder::Set function call. An owner \n
     should retrieve a configuration object via AlgoBase::GetConfig() function, \n
     then fill appropriate values. A child class should also implement AlgoBase::_Configure_ \n
     and AlgoBase::_Process_ functions: the former is to be called after configuration \n
     parameters are filled, and the latter is to be called per instance of \n
     trigger::WaveformArray_t object to make a trigger decision.
  */
  class AlgoBase{
    
  public:
    
    /**
       Constructor: this is where an algorithm should \n
       set configuration parameter names.
    */
  AlgoBase(const std::string name="noname") 
    : _cfg(name) , _time_profile(0) , _process_count(0) {}
    
    /// Default destructor
    virtual ~AlgoBase(){}

    /// Name getter (shares name w/ config object)
    const std::string& Name() const { return _cfg.Name(); }

    /// Config object getter
    ConfigHolder& GetConfig() { return _cfg; }

    /// Configure the algorithm
    void Configure();

    /// Run trigger algorithm
    const Result Process(const WaveformArray_t&);

    /// Time profile report (average time of execution over history)
    double AverageProcessTime() const;
    
  protected:

    /**
       Called by AlgoBase::Configure() internally. \n
       A virtual function to run algorithm configuration. \n
       This assumes configuration object has set values \n
       that an algorithm instance can retrieve and use. 
    */
    virtual void _Configure_() = 0;

    /// virtual function to run trigger algorithm and share result
    virtual const Result _Process_(const WaveformArray_t&) = 0;

    /// Algorithm configuration
    ConfigHolder _cfg;

  private:

    /// Boolean to flag if AlgoBase::Configure is called or not
    bool _configured;
    /// Watch for simple profiling
    trigger::Watch _watch;
    /// Process time using trigger::Watch
    double _time_profile;
    /// Process counter
    double _process_count;

    /// prescale factor
    int _prescale_factor;

    /// Prescale calculator
    //std::mt19937 _mt_rand;
    bool prescaleTrig(); ///< bool true if passed pre-scale

  };
}
#endif
/** @} */ // end of doxygen group 

