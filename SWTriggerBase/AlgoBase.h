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
#include <string>
#include <map>
#include <functional>
#ifndef __CINT__
#include <random>
#endif

#include "ConfigHolder.h"
#include "Result.h"
#include "Watch.h"
#include "AlgoFactory.h"

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
    const Result Process(unsigned int triggerbit, const WaveformArray_t&);

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
    virtual const Result _Process_(unsigned int triggerbit, const WaveformArray_t&) = 0;

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
    bool prescaleTrig(); ///< bool true if passed pre-scale

    /// Prescale RNG
#ifndef __CINT__
    std::mt19937 _randgen;
    std::function<float()> _randfunc;
#endif

  public:
    void setPrescaleSeed( int seed );

    /// ----------------
    /// abstract factory stuff

  private:
    static std::map< std::string, AlgoFactory* > _factories;
  public:
    static void registerConcreteFactory( const std::string& name, AlgoFactory* factory ) {
      _factories[ name ] = factory;
    };
    static AlgoBase* create( std::string algotype_name, std::string algoinstance_name ) {
      std::map< std::string, AlgoFactory* >::iterator it=_factories.find(algotype_name);
      if ( it==_factories.end() ) {
	char oops[500];
	sprintf(oops,"Algo type with name '%s' has not been registered.", algotype_name.c_str() );
	throw TriggerException(oops);
      }
      return (*it).second->create(algoinstance_name);
    };
    static void listAlgoTypes() {
      std::cout << "[AlgoFactories]" << std::endl;
      for ( auto it=_factories.begin(); it!=_factories.end(); it++ )
	std::cout << "  " << (*it).first << std::endl;
    };
    
  };
}
#endif
/** @} */ // end of doxygen group 

