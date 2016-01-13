#ifndef __TRIGGER_OUTPUT_BASE__
#define __TRIGGER_OUTPUT_BASE__

namespace uboonetrigger {

  class TriggerOutputBase {

  public:
    TriggerOutputBase() {};
    virtual ~TriggerOutputBase() {};

    // derived classes provide these
    virtual int getIntVar( std::string varname ) = 0;
    virtual float getFloatVar( std::string varname ) = 0;
    virtual short getShortVar( std::string varname ) = 0;
    
  };
  
}

#endif
