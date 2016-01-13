#ifndef __TRIGGER_CONFIG_BASE__
#define __TRIGGER_CONFIG_BASE__

#include <string>

namespace uboonetrigger {

  class TriggerConfigBase {

  public:
    // default ctor
    TriggerConfigBase() {};
    virtual ~TriggerConfigBase() {};
    
    virtual void setIntParameter( std::string parname, int value ) = 0;
    virtual void setFloatParameter( std::string parname, float value ) = 0;

  };

}

#endif
