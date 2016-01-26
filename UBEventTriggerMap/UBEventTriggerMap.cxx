#include "UBEventTriggerMap.h"

namespace trigger {

  bool UBEventTriggerMap::IsEventType( const ResultArray& results, UBEventTrigger_t eventtype ) {
    // This necessarily has hardcoded info about the trigger product
    for ( auto it=results.begin(); it!=results.end(); it++ ) {
      const Result& result = (*it);
      if ( IsCorrectAlgoInstanceName( result.algo_instance_name, eventtype ) )
	return true;
    }
    return false;
  }
  
  bool UBEventTriggerMap::IsEventTriggerType( const ResultArray& results, UBEventTrigger_t eventtype ) {
    // This necessarily has hardcoded info about the trigger product
    for ( auto it=results.begin(); it!=results.end(); it++ ) {
      const Result& result = (*it);
      if ( IsCorrectAlgoInstanceName( result.algo_instance_name, eventtype ) )
        return result.pass;
    }
    return false;
  }

  bool UBEventTriggerMap::IsCorrectAlgoInstanceName( std::string name, UBEventTrigger_t eventtype ) {
    switch (eventtype) {
      // BNB triggers
    case kBNBwFEMEmu:
      if ( name=="BNB_FEMBeamTriggerAlgo" ) return true;
      break;
    case kBNBunbiased:
      if ( name=="BNB_PrescaleAlgo" ) return true;
      break;
      // NUMI triggers
      // EXT triggers
    case kEXTwFEMEmuBNB:
      if ( name=="EXT_BNB_FEMBeamTriggerAlgo" ) return true;
      break;
    case kEXTwFEMEmuNUMI:
      if ( name=="EXT_NUMI_FEMBeamTriggerAlgo" ) return true;
      break;
    case kEXTunbiased:
      if ( name=="EXT_PrescaleAlgo" ) return true;
      break;
    default:
      return false;
      break;
    }
    return false; // never gets here
  }



}
