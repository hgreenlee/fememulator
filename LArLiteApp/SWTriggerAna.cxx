#ifndef LARLITE_SWTRIGGERANA_CXX
#define LARLITE_SWTRIGGERANA_CXX

#include "SWTriggerAna.h"
#include "DataFormat/mctruth.h"
#include "DataFormat/mctrack.h"
#include "DataFormat/mcshower.h"
#include "DataFormat/ophit.h"
#include "DataFormat/opflash.h"
#include "DataFormat/simphotons.h"
namespace larlite {

  bool SWTriggerAna::initialize() {

    if(!_alg) throw std::exception();

    _tree = new TTree("tree","");
    _tree->Branch("x",&_x,"x/D");
    _tree->Branch("y",&_y,"y/D");
    _tree->Branch("z",&_z,"z/D");
    _tree->Branch("t",&_t,"t/D");
    _tree->Branch("edep",&_edep,"edep/D");
    _tree->Branch("num_photons",&_num_photons,"num_photons/D");
    _tree->Branch("num_photons_total",&_num_photons_total,"num_photons_total/D");
    _tree->Branch("num_pe",&_num_pe,"num_pe/D");
    _tree->Branch("num_pe_prompt",&_num_pe_prompt,"num_pe_prompt/D");
    _tree->Branch("flash_pe",&_flash_pe,"flash_pe/D");
    _tree->Branch("flash_time",&_flash_time,"flash_time/D");
    _tree->Branch("phmax",&_phmax,"phmax/D");
    _tree->Branch("fired",&_fired,"fired/I");
    _tree->Branch("tick",&_tick,"tick/D");
    _tree->Branch("pdg",&_pdg,"pdg/I");

    return true;
  }
  
  bool SWTriggerAna::analyze(storage_manager* storage) {

    auto ev_opdigit = storage->get_data<event_opdetwaveform>("pmtreadout");
    if(!ev_opdigit) throw DataFormatException("opdigit not found!");

    bool processed = false;
    try{
      auto res = _alg->Emulate(*ev_opdigit);
      _fired = int(res.pass);
      _phmax = res.amplitude;
      _tick  = res.time;
      processed = true;
    }catch(...) {
      _fired = -1;
      _phmax = -1;
    }

    auto ev_mct = storage->get_data<event_mctruth>("generator");
    if(!ev_mct) throw DataFormatException("mctruth not found!");

    auto const& mct = ev_mct->front();
    double time_integral_start, time_integral_end;
    
    if(mct.NeutrinoSet()) {
      auto const& mcp_v = mct.GetParticles();
      for(auto const& mcp : mcp_v) {
	if(mcp.StatusCode()!=1) continue;
	_x = mcp.Trajectory().front().X();
	_y = mcp.Trajectory().front().Y();
	_z = mcp.Trajectory().front().Z();
	_t = mcp.Trajectory().front().T()/1.e3;
	break;
      }
      _pdg = 14;
      //time_integral_start = 0.34375 + 200*0.015625;
      time_integral_start = 0.34375 + _t/1000. - 0.01;
      time_integral_end   = time_integral_start + 8;
    }else{
      auto const& mcp = mct.GetParticles().front();
      _x = mcp.Trajectory().front().X();
      _y = mcp.Trajectory().front().Y();
      _z = mcp.Trajectory().front().Z();
      _t = mcp.Trajectory().front().T()/1.e3;
      time_integral_start = _t/1000. - 0.01;
      time_integral_end   = time_integral_start + 8;
      _pdg = mcp.PdgCode();
    }

    //auto ev_simph = storage->get_data<event_simphotons>("largeant");
    //if(!ev_simph) throw DataFormatException("SimPhotons not found!");
    _num_photons = _num_photons_total = 0;
    /*
    for(auto const& simph : *ev_simph) {
      _num_photons_total += simph.size();
      for(auto const& oneph : simph)
	if( time_integral_start * 1.e3 < oneph.Time && oneph.Time < time_integral_end * 1.e3)
	  _num_photons += 1;
    }
    */
    auto ev_ophit = storage->get_data<event_ophit>("opflashSat");
    if(!ev_ophit) throw DataFormatException("OpHit not found!");
    _num_pe = 0;
    _num_pe_prompt=0;
    for(auto const& oph : *ev_ophit) {

      if(oph.OpChannel()>=32) continue;

      if(oph.PeakTime() < time_integral_start || oph.PeakTime() > time_integral_end) continue;

      if(oph.PeakTime() < (time_integral_start + 0.5)) _num_pe_prompt += oph.PE();
      _num_pe += oph.PE();
    }

    auto ev_opflash = storage->get_data<event_opflash>("opflashSat");
    if(!ev_opflash) throw DataFormatException("OpFlash not found!");
    _flash_pe = 0;
    _flash_time = 1e12;
    for(auto const& f : *ev_opflash) {

      if(f.Time() < (time_integral_start -1.) || f.Time() > (time_integral_end +1.)) continue;

      if(_flash_pe < f.TotalPE()) {
	_flash_pe = f.TotalPE();
	_flash_time = f.Time();
      }
      
    }

    auto ev_mcshower = storage->get_data<event_mcshower>("mcreco");
    if(!ev_mcshower) throw DataFormatException("MCShower not found!");

    auto ev_mctrack = storage->get_data<event_mctrack>("mcreco");
    if(!ev_mctrack) throw DataFormatException("MCTrack not found!");

    _edep = 0;
    for(auto const& mcs : *ev_mcshower) _edep += mcs.DetProfile().E();
    for(auto const& mct : *ev_mctrack ) {

      if(mct.size()<2) continue;

      _edep += (mct.front().E() - mct.back().E());

    }

    _tree->Fill();
    
    return true;
  }

  bool SWTriggerAna::finalize() {

    if(_fout) {
      _fout->cd();
      _tree->Write();
    }
  
    return true;
  }

}
#endif
