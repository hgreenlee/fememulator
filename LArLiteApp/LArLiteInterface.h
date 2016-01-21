#ifndef __FEMEMULATOR_LARLITE_INTERFACE_H
#define __FEMEMULATOR_LARLITE_INTERFACE_H
#include <vector>
#include "FEMBeamTrigger/FEMBeamTriggerAlgo.h"
#include "DataFormat/opdetwaveform.h"
#include "DataFormat/fifo.h"
namespace trigger {

  namespace fememu {

    /// LArLite interface class for fememu::FEMBeamTriggerAlgo class
    class LLInterface : public trigger::fememu::FEMBeamTriggerAlgo {
    public:
      /// Default ctor
      LLInterface() : trigger::fememu::FEMBeamTriggerAlgo() { _FEMSlot = 5; }
      /// Config ctor
      LLInterface(const FEMBeamTriggerConfig& cfg) : trigger::fememu::FEMBeamTriggerAlgo(cfg) {}
      /// Default dtor
      ~LLInterface(){}
      
      /// Add an interface type that accepts larlite::event_opdetwaveform
      const trigger::Result Emulate( const ::larlite::event_opdetwaveform& );
      
      /// Add an interface type that accepts larlite::event_opdetwaveform
      const trigger::Result Emulate( const ::larlite::event_fifo& );

      /// setter for FEM slot to be used when using FIFO data-product
      void setFEMSlot(int slot) { _FEMSlot = slot; }

    private:
      
      /// FEM slot to be used
      int _FEMSlot;
      
    };
  }
}

#endif
