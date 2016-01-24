#include <iostream>

#include "SWTriggerBase/AlgoBase.h"
#include "SWTriggerBase/MultiAlgo.h"

int main( int narg, char** argv ) {

  std::cout << "Test App" << std::endl;

  trigger::MultiAlgo algos;
  trigger::AlgoBase::listAlgoTypes();

  int trigbit = 0x200;
  algos.declareAlgo( trigbit, "FEMBeamTriggerAlgo", "swtrg_bnb" );
  algos.declareAlgo( trigbit, "PrescaleAlgo", "prescale_bnb" );

  algos.GetConfig("swtrg_bnb").Set("Verbosity",3,true);
  algos.GetConfig("swtrg_bnb").Set("MinReadoutTick",500,true);
  algos.GetConfig("swtrg_bnb").Set("Discr0precount",1,true);
  algos.GetConfig("swtrg_bnb").Set("Discr0delay",4,true);
  algos.GetConfig("swtrg_bnb").Set("Discr0threshold",5,true);
  algos.GetConfig("swtrg_bnb").Set("Discr0deadtime",6,true);
  algos.GetConfig("swtrg_bnb").Set("Discr0precount",1,true);
  algos.GetConfig("swtrg_bnb").Set("Discr3delay",4,true);
  algos.GetConfig("swtrg_bnb").Set("Discr3threshold",10,true);
  algos.GetConfig("swtrg_bnb").Set("Discr3deadtime",6,true);
  algos.GetConfig("swtrg_bnb").Set("Discr3width",6,true);
  algos.GetConfig("swtrg_bnb").Set("Discr3WindowStart",54,true);
  algos.GetConfig("swtrg_bnb").Set("Discr3WindowSize",105,true);
  algos.GetConfig("swtrg_bnb").Set("TriggerThresMult",1,true);
  algos.GetConfig("swtrg_bnb").Set("TriggerThresPHMAX",2000,true);
  algos.GetConfig("swtrg_bnb").Set("TriggerWindowStart",57,true);
  algos.GetConfig("swtrg_bnb").Set("TriggerWindowSize",104,true);
  algos.GetConfig("swtrg_bnb").Set("PrescaleFactor",1,true);

  algos.GetConfig("prescale_bnb").Set("PrescaleFactor",2,true);

  algos.Configure();
  algos.GetAlgo("prescale_bnb").setPrescaleSeed( 100 );

  std::cout << "PHMAX: " << algos.GetConfig("swtrg_bnb").Get<int>("TriggerThresPHMAX") << std::endl;
  std::cout << "Prescale Factor: " << algos.GetConfig("prescale_bnb").Get<int>("PrescaleFactor") << std::endl;

  int nwaveforms = 500;
  int testwidth = 30;
  int nchannels = 32;
  int nticks = 1500;
  int pulseheight = 3000;

  std::cout << "Make waveforms" << std::endl;
  for (int iwaveform=0; iwaveform<nwaveforms; iwaveform++) {
    std::vector< std::vector<unsigned short> > testwfms(nchannels);
    for (int ich=0; ich<nchannels; ich++) {
      testwfms.at(ich).resize( nticks, 0 );
      for (int t=iwaveform; t<iwaveform+testwidth; t++) {
	testwfms.at(ich).at(t) = pulseheight;
      }
    }
  
    int bit = 0x200;
    trigger::ResultArray results = algos.Process( bit, testwfms );
    
    std::cout << "[Waveform " << iwaveform << "] passedone=" << results.passedone << " passedall=" << results.passedall << std::endl;
    for ( auto it=results.begin(); it!=results.end(); it++ ) {
      std::cout << " " << (*it).algo_instance_name << ": passed=" << (*it).pass << " tick=" << (*it).time << " PHMAX=" << (*it).amplitude << std::endl;
    }
  }

  return 0;
}
