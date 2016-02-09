/**
 * \file SWTriggerAna.h
 *
 * \ingroup LArLiteApp
 * 
 * \brief Class def header for a class SWTriggerAna
 *
 * @author kazuhiro
 */

/** \addtogroup LArLiteApp

    @{*/

#ifndef LARLITE_SWTRIGGERANA_H
#define LARLITE_SWTRIGGERANA_H

#include "Analysis/ana_base.h"
#include "LArLiteInterface.h"
#include <TTree.h>
namespace larlite {
  /**
     \class SWTriggerAna
     User custom analysis class made by SHELL_USER_NAME
   */
  class SWTriggerAna : public ana_base{
  
  public:

    /// Default constructor
    SWTriggerAna(){ _name="SWTriggerAna"; _fout=0;}

    /// Default destructor
    virtual ~SWTriggerAna(){}

    /** IMPLEMENT in SWTriggerAna.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in SWTriggerAna.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in SWTriggerAna.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

    void SetAlgo(::trigger::fememu::LLInterface* ptr) { _alg = ptr; }

  private:

    ::trigger::fememu::LLInterface* _alg;

    TTree *_tree;
    double _x;
    double _y;
    double _z;
    double _t;
    double _edep;
    double _num_photons;
    double _num_photons_total;
    double _num_pe;
    double _num_pe_prompt;
    double _flash_pe;
    double _flash_time;
    double _phmax;
    double _tick;
    int    _fired;
    int    _pdg;
    
  protected:
    
  };
}
#endif

//**************************************************************************
// 
// For Analysis framework documentation, read Manual.pdf here:
//
// http://microboone-docdb.fnal.gov:8080/cgi-bin/ShowDocument?docid=3183
//
//**************************************************************************

/** @} */ // end of doxygen group 
