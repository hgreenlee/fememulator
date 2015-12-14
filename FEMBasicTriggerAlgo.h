#ifndef __FEMBASICTRIGGERALGO__
#define  __FEMBASICTRIGGERALGO__

#include <vector>
#include "FEMInterface.h"
#include <iostream>

namespace fememu {

  class BasicTriggerConfig {

  public:
    BasicTriggerConfig() {};
    virtual ~BasicTriggerConfig() {};
    
    bool fVerbose;
    bool fSetBeamWindow;

    short fDiscr0delay;
    short fDiscr3delay;
    
    short fDiscr0threshold;
    short fDiscr3threshold;

    short fDiscr0precount;
    
    short fDiscr0deadtime;
    short fDiscr3deadtime;
    
    short fDiscr0width;
    short fDiscr3width;

    short fBeamWinSize;
    short fBeamWinStartTick;
    short fMinReadoutTicks;
    short fFrontBuffer;
    short fWindowSize;

    void print() const {
      std::cout << "[fememu::basicTrigger] === Parameters ===" << std::endl;
      if ( fSetBeamWindow )
	std::cout << " BeamWinStartTick: " << fBeamWinStartTick << std::endl;
      else
	std::cout << " BeamWinSize: " << fBeamWinSize << std::endl;
      std::cout << " MinReadoutTicks: " << fBeamWinSize << std::endl;
      std::cout << " Front Buffer: " << fFrontBuffer << std::endl;
      std::cout << " Discr 0: " <<  fDiscr0threshold << " " << fDiscr0deadtime << " " << fDiscr0deadtime << std::endl;
      std::cout << " Discr 3: " <<  fDiscr3threshold << " " << fDiscr3deadtime << " " << fDiscr3deadtime << std::endl;
    }

  };


  TriggerOutput basicTrigger( const BasicTriggerConfig& config, const WaveformStore& wfms );

}

#endif
