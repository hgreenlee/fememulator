#ifndef __UBEVENTTRIGGERMAP__
#define __UBEVENTTRIGGERMAP__

#include "UBEventTriggerTypes.h"
#include "SWTriggerBase/Result.h"

#include <string>

namespace trigger {

  class UBEventTriggerMap {

  public:
    
    UBEventTriggerMap() {};
    virtual ~UBEventTriggerMap() {};

    static bool IsEventType( const ResultArray&, UBEventTrigger_t );
    static bool IsEventTriggerType( const ResultArray& results, UBEventTrigger_t eventtype );
    static bool IsCorrectAlgoInstanceName( std::string name, UBEventTrigger_t eventtype );

    // here add preprocessor flag for datatypes?

  };


}


#endif
