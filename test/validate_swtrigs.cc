#include <iostream>
#include <string>
#include <vector>

#include "TriggerDAQApp/DAQFileInterface.h"
#include "SWTriggerBase/SWTriggerTypes.h"
#include "SWTriggerBase/MultiAlgo.h"

#include "TFile.h"
#include "TH1D.h"
#include "TGraph.h"

int main( int nargs, char** argv ) {

  std::cout << "VALIDATE SWTRIGS" << std::endl;

  std::vector< std::string > inputfiles;
  inputfiles.push_back( "/home/tmw/run4692_multialgotest_extonly/TestRun-2016_1_25_16_24_52-0004692-00000.ubdaq" );

  trigger::DAQFileInterface data;

  for ( auto &f : inputfiles ) {
    std::cout << "Adding Input file: " << f << std::endl;
    data.AddInputFile( f );
  }

  data.SetTarget(5,0,32,700);
  data.Initialize();
  
  //============================================
  //  Configure Algorithms
  //  We need a fhl parser
  trigger::MultiAlgo algos;
  //trigger_bits();
  int ext = 0x200;
  int bnb = 0x800;
  
  // Declare Algos
  algos.declareAlgo( ext, "FEMBeamTriggerAlgo", "swtrg_ext" );
  algos.declareAlgo( bnb, "FEMBeamTriggerAlgo", "swtrg_bnb" );
  
  // Set parameters
  // BNB
  algos.GetConfig("swtrg_bnb").Set("Verbosity",3,true);
  algos.GetConfig("swtrg_bnb").Set("MinReadoutTick",500,true);
  algos.GetConfig("swtrg_bnb").Set("Discr0precount",1,true);
  algos.GetConfig("swtrg_bnb").Set("Discr0delay",4,true);
  algos.GetConfig("swtrg_bnb").Set("Discr0threshold",5,true);
  algos.GetConfig("swtrg_bnb").Set("Discr0deadtime",6,true);
  algos.GetConfig("swtrg_bnb").Set("Discr3delay",4,true);
  algos.GetConfig("swtrg_bnb").Set("Discr3threshold",10,true);
  algos.GetConfig("swtrg_bnb").Set("Discr3deadtime",6,true);
  algos.GetConfig("swtrg_bnb").Set("Discr3width",6,true);
  algos.GetConfig("swtrg_bnb").Set("Discr3WindowStart",54,true);
  algos.GetConfig("swtrg_bnb").Set("Discr3WindowSize",105,true);
  algos.GetConfig("swtrg_bnb").Set("TriggerThresMult",1,true);
  algos.GetConfig("swtrg_bnb").Set("TriggerThresPHMAX",70,true);
  algos.GetConfig("swtrg_bnb").Set("TriggerWindowStart",57,true);
  algos.GetConfig("swtrg_bnb").Set("TriggerWindowSize",104,true);
  algos.GetConfig("swtrg_bnb").Set("PrescaleFactor",1,true);

  algos.GetConfig("swtrg_ext").Set("Verbosity",3,true);
  algos.GetConfig("swtrg_ext").Set("MinReadoutTick",500,true);
  algos.GetConfig("swtrg_ext").Set("Discr0precount",1,true);
  algos.GetConfig("swtrg_ext").Set("Discr0delay",4,true);
  algos.GetConfig("swtrg_ext").Set("Discr0threshold",5,true);
  algos.GetConfig("swtrg_ext").Set("Discr0deadtime",6,true);
  algos.GetConfig("swtrg_ext").Set("Discr3delay",4,true);
  algos.GetConfig("swtrg_ext").Set("Discr3threshold",10,true);
  algos.GetConfig("swtrg_ext").Set("Discr3deadtime",6,true);
  algos.GetConfig("swtrg_ext").Set("Discr3width",6,true);
  algos.GetConfig("swtrg_ext").Set("Discr3WindowStart",54,true);
  algos.GetConfig("swtrg_ext").Set("Discr3WindowSize",105,true);
  algos.GetConfig("swtrg_ext").Set("TriggerThresMult",1,true);
  algos.GetConfig("swtrg_ext").Set("TriggerThresPHMAX",70,true);
  algos.GetConfig("swtrg_ext").Set("TriggerWindowStart",57,true);
  algos.GetConfig("swtrg_ext").Set("TriggerWindowSize",104,true);
  algos.GetConfig("swtrg_ext").Set("PrescaleFactor",1,true);


  algos.Configure();
  //============================================

  // Output
  TFile* output = new TFile( "output_validate_swtrigs.root", "RECREATE" );
  TH1D* heff_online  = new TH1D("heff_online", ";PHMAX/20;fraction passed",  50, 1, 26);
  TH1D* heff_offline = new TH1D("heff_offline", ";PHMAX/20;fraction passed", 50, 1, 26);
  heff_online->Sumw2(true);
  heff_offline->Sumw2(true);


  // counters
  int counters[15] = {0};
  float thresholds[15] = { 0.5, 1.5, 2.5, 3.5, 4.5,
			   5.5, 6.5, 7.5, 8.5, 9.5,
			   14.5, 19.5, 24.5, 29.5, 34.5 }; 
  
  int nevents = 0;
  while ( data.ProcessEvent() ) {
    std::cout << "[ Run " << data.run() << " Subrun " << data.subrun() << " Event " << data.event() << " ]" << std::endl; 
    const trigger::WaveformArray_t& wfms = data.WaveformArray();
    const trigger::ResultArray& results = data.GetTriggerResults();
    trigger::ResultArray offline = algos.Process( data.trigger_bits(), wfms );
    int  offline_phmax = offline.at(0).amplitude;
    int  offline_multi = offline.at(0).multiplicity;
    bool offline_pass  = offline.at(0).pass_algo;
    std::cout << "  trigger bit: " << data.trigger_bits() << std::endl;
    std::cout << "  number of pmt wfms: " << wfms.size() << std::endl;
    std::cout << "  number of online triggers: " << results.size() << " [passed one=" << results.passedone << "]" << std::endl;
    std::cout << "  number of offline triggers: " << offline.size() << " (" << offline.at(0).algo_instance_name << ")" << std::endl;
    int online_phmax = 0;
    int online_multi = 0;
    float online_weight = 1.0;
    bool online_pass = 0;
    bool online_passedone = false;
    for ( auto &result : results ) {
      std::cout << "  " << result.algo_instance_name 
		<< "  pass=" << result.pass 
		<< " algo=" << result.pass_algo  << " [offline=" << offline_pass << "]"
		<< " prescale=" << result.pass_prescale
		<< "  tick=" << result.time << " [offline=" << offline.at(0).time << "]"
		<< "  phmax=" << result.amplitude << " [offline=" << offline_phmax << "]"
		<< "  multi=" << result.multiplicity << " [offline=" << offline_multi << "]"
		<< "  weight=" << result.prescale_weight
		<< std::endl;
      if ( result.pass )
	online_passedone = true;
      if ( result.algo_instance_name=="EXT_BNB_FEMBeamTriggerAlgo" ) {
	online_phmax = result.amplitude;
	online_multi = result.multiplicity;
	online_pass = result.pass;
      }
      else if ( result.algo_instance_name=="EXT_PrescaleAlgo" ) {
	online_weight = result.prescale_weight;
      }
    }

    // now fill
    
    // we can fill the offline for every event
    int bin = heff_offline->FindBin( float(offline_phmax)/20.0 );
    for ( int b=1; b<=bin; b++) {
      heff_offline->Fill( heff_offline->GetBinCenter(b) );
    }

    // online fills differently
    if ( online_pass || (!online_pass && online_passedone) ) {
      int phmax_fill = online_phmax;
      float weight_fill = 1.0;
      if ( !online_pass ) {
	// passed via prescale
	weight_fill = online_weight;
	phmax_fill = offline_phmax;
      }
      for ( int b=1; b<=heff_online->GetNbinsX(); b++) {
	if ( float(phmax_fill)/20.0>=heff_online->GetBinCenter(b) )
	  heff_online->Fill( heff_online->GetBinCenter(b), weight_fill );
      }      
    }

    // counters
    for (int n=0; n<15; n++) {
      if ( float(offline_phmax)/20.0>=thresholds[n] ) {
	counters[n]++;
      }
    }
    
    nevents++;
    //if (nevents>100)
    //break;
  }
  
  heff_offline->Scale(1.0/float(nevents));
  heff_online->Scale(1.0/float(nevents));

  TGraph* goffline = new TGraph( 15 );
  for (int n=0; n<15; n++) {
    goffline->SetPoint( n, thresholds[n], float(counters[n])/float(nevents) );
  }
  goffline->Write("geff_offline");

  output->Write();
  return 0;
}
