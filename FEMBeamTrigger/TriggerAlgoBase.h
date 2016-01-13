/* 

   Abstract base class that defines interface to trigger algorithm.

 */

#ifndef __TRIGGER_ALGO_BASE__
#define __TRIGGER_ALGO_BASE__

#include <memory>
#include "TriggerConfigBase.h"
#include "TriggerOutputBase.h"

namespace uboonetrigger {

  class TriggerAlgoBase {

  public:
    /// Default ctor
    TriggerAlgoBase() {};
    /// Config ctor
#ifndef __MAKECINT__
    TriggerAlgoBase( std::unique_ptr<TriggerConfigBase> cfg ) { setConfig(std::move(cfg)); };
#endif
    /// Default dtor
    virtual ~TriggerAlgoBase() {};
    
    const TriggerConfigBase& cfg() { return *_cfg; };

#ifndef __MAKECINT__    
    void setConfig( std::unique_ptr<TriggerConfigBase> cfg ) { _cfg = std::move(cfg); };
#endif
    virtual std::unique_ptr<uboonetrigger::TriggerOutputBase> Evaluate( const WaveformArray_t& ) = 0;
    
  protected:
#ifndef __MAKECINT__    
    std::unique_ptr<TriggerConfigBase> _cfg;
#endif
    
  };    
}
#endif
