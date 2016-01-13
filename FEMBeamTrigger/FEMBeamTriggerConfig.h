#ifndef __FEM_BEAM_TRIGGER_CONFIG__
#define __FEM_BEAM_TRIGGER_CONFIG__

#include "TriggerAlgoTypes.h"
#include "TriggerConfigBase.h"

namespace uboonetrigger {
  
  class FEMBeamTriggerConfig : public TriggerConfigBase {

    
  public:

    int fVerbosity;
    short fMinReadoutTicks;  ///< Ignore waveform if has ticks less than this number. Used to separate cosmic windows and beam windows

    short fDiscr0precount;  ///< Precount for disc. 0 to fire
    short fDiscr0delay;     ///< Delay to form a diff vector
    short fDiscr0deadtime;  ///< Deadtime for disc. 0
    short fDiscr0threshold; ///< Disc. 0 threshold
    
    short fDiscr3width;     ///< Width for disc. 3
    short fDiscr3delay;     ///< Delay to form a diff vector
    short fDiscr3deadtime;  ///< Deadtime for disc. 3 (does not exist! FIX ME)
    short fDiscr3threshold; ///< Disc. 3 threshold

    short fDiscr3WindowStart; ///< tick where Disc3 fire-allowed window start
    short fDiscr3WindowSize;  ///< size of Disc3 fire-allowed window

    short fTriggerWindowStart; ///< start of PMT Trigger Window
    short fTriggerWindowSize;  ///< size of PMT Trigger Window in ticks

    short fTriggerThresMult;    ///< FEM beam trigger threshold (multiplicity)
    short fTriggerThresPHMAX;   ///< FEM beam trigger threshold (PHMAX sum)

  public:

    FEMBeamTriggerConfig()
      {
	// Note: For BNB Events
	fVerbosity = kNORMAL;
      
	fDiscr0delay = 4;
	fDiscr0threshold = 5;
	fDiscr0deadtime = 6;
	fDiscr0precount = 3;
	
	fDiscr3delay = 4;
	fDiscr3threshold = 10;
	fDiscr3deadtime = 6;
	fDiscr3width = 6;
	
	fDiscr3WindowStart = 100;
	fDiscr3WindowSize  = 104;
	
	fTriggerWindowStart = 100;
	fTriggerWindowSize  = 104;
	
	fMinReadoutTicks = 500;
	fTriggerThresMult  = 4;
	fTriggerThresPHMAX = 80;
      };
    
    virtual ~FEMBeamTriggerConfig() {};

    // implementing this
    virtual void setIntParameter( std::string parname, int value ) {
      if ( parname=="fDiscr0delay" ) fDiscr0delay = value;
      else if ( parname=="fDiscr0threshold" ) fDiscr0threshold = value;
      else if ( parname=="fDiscr0deadtime" ) fDiscr0deadtime = value;
      else if ( parname=="fDiscr0precount" ) fDiscr0precount = value;
      else if ( parname=="fDiscr0precount" ) fDiscr0precount = value;
      else if ( parname=="fDiscr3delay" ) fDiscr3delay = value;
      else if ( parname=="fDiscr3threshold" ) fDiscr3threshold = value;
      else if ( parname=="fDiscr3deadtime" ) fDiscr3deadtime = value;
      else if ( parname=="fDiscr3width" ) fDiscr3width = value;
      else if ( parname=="fDiscr3WindowStart" ) fDiscr3WindowStart = value;
      else if ( parname=="fDiscr3WindowSize" ) fDiscr3WindowSize = value;
      else if ( parname=="fTriggerWindowStart" ) fTriggerWindowStart= value;
      else if ( parname=="fTriggerWindowSize" ) fTriggerWindowSize = value;
      else if ( parname=="fMinReadoutTicks" ) fMinReadoutTicks = value;
      else if ( parname=="fTriggerThresMult" ) fTriggerThresMult = value;
      else if ( parname=="fTriggerThresPHMAX" ) fTriggerThresPHMAX = value;
      else {
	std::string warning = std::string("Parameter, ") + parname + std::string(", not recognized");
	throw warning;
      }
    };

    virtual void setFloatParameter( std::string parname, float value ) {
      throw "FEMBeamTriggerConfig does not have any float arguments";
    };

  };

}

#endif
