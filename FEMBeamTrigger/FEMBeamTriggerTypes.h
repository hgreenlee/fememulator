#ifndef __FEMBEAMTRIGGERTYPES__
#define  __FEMBEAMTRIGGERTYPES__

#include <vector>
#include <string>
#include <exception>

namespace trigger {
  namespace fememu {
    
    /// Message level for an algorithm
    enum MessageLevel_t {
      kDEBUG,
      kINFO,
      kNORMAL,
      kWARNING,
      kERROR,
      kCRITICAL
    };

    /**
       Beam FEM Trigger configuration parameter holder struct
    */
    struct FEMBeamTriggerConfig {
    public:
      
      MessageLevel_t fVerbosity; ///< Verbosity level
      
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
      
      /// Default ctor
      FEMBeamTriggerConfig()
      {
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
      }
      
    };

  }
}

#endif
