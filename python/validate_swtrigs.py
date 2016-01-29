

  std::cout << "VALIDATE SWTRIGS" << std::endl;

  std::vector< std::string > inputfiles;

  // EXT only events
  inputfiles.push_back( "/home/tmw/run4692_multialgotest_extonly/TestRun-2016_1_25_16_24_52-0004692-00000.ubdaq" );
  // inputfiles.push_back( "/home/tmw/run4692_multialgotest_extonly/TestRun-2016_1_25_16_24_52-0004692-00001.ubdaq" );
  // inputfiles.push_back( "/home/tmw/run4692_multialgotest_extonly/TestRun-2016_1_25_16_24_52-0004692-00002.ubdaq" );
  // inputfiles.push_back( "/home/tmw/run4692_multialgotest_extonly/TestRun-2016_1_25_16_24_52-0004692-00003.ubdaq" );
  // inputfiles.push_back( "/home/tmw/run4692_multialgotest_extonly/TestRun-2016_1_25_16_24_52-0004692-00004.ubdaq" );

  // EXT+BNB events
  // inputfiles.push_back( "/home/tmw/run4693_multialgotest_extbnb/TestRun-2016_1_25_16_37_3-0004693-00000.ubdaq" );
  // inputfiles.push_back( "/home/tmw/run4693_multialgotest_extbnb/TestRun-2016_1_25_16_37_3-0004693-00001.ubdaq" );
  // inputfiles.push_back( "/home/tmw/run4693_multialgotest_extbnb/TestRun-2016_1_25_16_37_3-0004693-00002.ubdaq" );
  // inputfiles.push_back( "/home/tmw/run4693_multialgotest_extbnb/TestRun-2016_1_25_16_37_3-0004693-00003.ubdaq" );
  // inputfiles.push_back( "/home/tmw/run4693_multialgotest_extbnb/TestRun-2016_1_25_16_37_3-0004693-00004.ubdaq" );
  // inputfiles.push_back( "/home/tmw/run4693_multialgotest_extbnb/TestRun-2016_1_25_16_37_3-0004693-00005.ubdaq" );

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
  algos.GetConfig("swtrg_bnb").Set("PrescaleFactor",1.0,true);

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
  algos.GetConfig("swtrg_ext").Set("PrescaleFactor",1.0,true);


  algos.Configure();
  //============================================

  // Output
  TFile* output = new TFile( "output_validate_swtrigs.root", "RECREATE" );


  // counters
  float thresholds[15] = { 0.5, 1.5, 2.5, 3.5, 4.5,
			   5.5, 6.5, 7.5, 8.5, 9.5,
			   14.5, 19.5, 24.5, 29.5, 34.5 }; 
  std::map< std::string, Tracker > counters;
  counters["ext_online"] = Tracker();
  counters["ext_offline"] = Tracker();
  counters["bnb_online"] = Tracker();
  counters["bnb_offline"] = Tracker();
  
  int ndecision_disagree = 0;
  TH1D* hphmaxdiff = new TH1D("hphmaxdiff",";dphmax(online-offline);",100, -20, 20 );
  
  int nevents = 0;
  while ( data.ProcessEvent() ) {
    std::cout << "[ Run " << data.run() << " Subrun " << data.subrun() << " Event " << data.event() << " ]" << std::endl; 
    // Waveforms for analysis
    const trigger::WaveformArray_t& wfms = data.WaveformArray();

    // Output of Trigger run by DAQ
    const trigger::ResultArray& online_results = data.GetTriggerResults();

    // Output of Trigger run offline
    trigger::ResultArray offline_results = algos.Process( data.trigger_bits(), wfms );
    int  offline_phmax = offline_results.at(0).amplitude;
    int  offline_multi = offline_results.at(0).multiplicity;
    bool offline_pass  = offline_results.at(0).pass_algo;
    std::cout << "  trigger bit: " << data.trigger_bits() << std::endl;
    std::cout << "  number of pmt wfms: " << wfms.size() << std::endl;
    std::cout << "  number of online triggers: " << online_results.size() << " [passed one=" << online_results.passedone << "]" << std::endl;
    std::cout << "  number of offline triggers: " << offline_results.size() << " (" << offline_results.at(0).algo_instance_name << ")" << std::endl;
    int online_phmax = 0;
    int online_multi = 0;
    float online_weight = 1.0;
    bool online_pass = 0;
    bool online_passedone = false;
    std::string hwtrigtype; // bnb or ext
    for ( auto &result : online_results ) {
      std::cout << "  " << result.algo_instance_name 
		<< "  pass=" << result.pass 
		<< " algo=" << result.pass_algo  << " [offline=" << offline_pass << "]"
		<< " prescale=" << result.pass_prescale
		<< "  tick=" << result.time << " [offline=" << offline_results.at(0).time << "]"
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
	hwtrigtype = "ext";
      }
      else if ( result.algo_instance_name=="EXT_PrescaleAlgo" ) {
	online_weight = result.prescale_weight;
	hwtrigtype = "ext";
      }
      else if ( result.algo_instance_name=="BNB_FEMBeamTriggerAlgo" ) {
	online_phmax = result.amplitude;
	online_multi = result.multiplicity;
	online_pass = result.pass;
	hwtrigtype = "bnb";	
      }
      else if ( result.algo_instance_name=="BNB_PrescaleAlgo" ) {
	online_weight = result.prescale_weight;
	hwtrigtype = "bnb";
      }
    }

    // track disagreements
    if ( online_pass!=offline_pass ) {
      ndecision_disagree++;
    }

    if ( online_pass ) {
      hphmaxdiff->Fill( online_phmax-offline_phmax );
    }

    // now fill the counters

    // offline counters
    Tracker& trackoff = counters[hwtrigtype+"_offline"];
    for (int n=0; n<15; n++) {
      if ( float(offline_phmax)/20.0>=thresholds[n] ) {
	trackoff.counters[n] += 1.0;
	trackoff.w2[n] += 1.0;
      }
    }
    trackoff.fills += 1.0;

    // online counters: we have to weight up events that fail online trigger algo, but pass online
    Tracker& trackon = counters[hwtrigtype+"_online"];
    if ( online_passedone ) {
      float fillweight = 1.0;
      float fillphmax = online_phmax;
      if ( !online_pass ) {
	// then the prescale triggered the event
	fillphmax = offline_phmax; // because if fails, online doesn't store a phmax value (should correct this?)
	fillweight = (float)online_weight;
      }
      for (int n=0; n<15; n++) {
	if ( float(fillphmax)/20.0>=thresholds[n] ) {
	  trackon.counters[n] += fillweight;
	  trackon.w2[n] += fillweight*fillweight;
	}
      }
    }
    trackon.fills += 1.0;
    
    nevents++;
    //if (nevents>100)
    //break;
  }
  
  // fill graphs!
  for ( auto it=counters.begin(); it!=counters.end(); it++ ) {
    TGraphErrors* goffline = new TGraphErrors( 15 ); // needs to be tgrapherrrors
    Tracker& out = (*it).second;
    if ( out.fills>0 ) {
      for (int n=0; n<15; n++) {
	goffline->SetPoint( n, thresholds[n], float(out.counters[n])/out.fills );
	goffline->SetPointError( n, 0, sqrt(out.w2[n])/out.fills );
      }
    }
    goffline->Write((*it).first.c_str());
  }

  output->Write();

  std::cout << "NUMBER OF DISAGREEMENTS: " << ndecision_disagree << std::endl;
  return 0;
}
