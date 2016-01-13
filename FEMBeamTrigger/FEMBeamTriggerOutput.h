#ifndef __FEM_BEAM_TRIGGER_OUTPUT__
#define __FEM_BEAM_TRIGGER_OUTPUT__

#include "TriggerOutputBase.h"

namespace uboonetrigger {

  class FEMBeamTriggerOutput : public TriggerOutputBase {

  public:
    
  FEMBeamTriggerOutput() 
    : window_start (  0 )
      , window_end   (  0 )
      , vmaxdiff     (  0 )
      , vmaxhit      (  0 )
      , fire_time    ( -1 )
      {};
    virtual ~FEMBeamTriggerOutput() {};

    virtual int getIntVar( std::string varname ) {
      if ( varname=="PHMAX" ) return vmaxdiff;
      else if ( varname=="MULTI" ) return vmaxhit;
      else if ( varname=="TriggerTick" ) return fire_time;
      else {
	throw "unrecognized int variable";
      }
    };
    virtual short getShortVar( std::string varname ) {
      if ( varname=="decision" ) return decision;
      else {
	throw "unrecognized short variable";
      }
    };
    virtual float getFloatVar( std::string varname ) {
      throw "no float variables";
    };

    size_t window_start; ///< Window start tick
    size_t window_end;   ///< Window end   tick
    int  vmaxdiff;     ///< PHMAX sum
    int  vmaxhit;      ///< Multiplicity sum
    int  fire_time;    ///< Trigger decision times (-1 means not fired)
    int decision;

  };

}

#endif
