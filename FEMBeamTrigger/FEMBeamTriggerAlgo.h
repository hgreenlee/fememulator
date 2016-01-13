#ifndef __FEMBEAMTRIGGERALGO_H__
#define __FEMBEAMTRIGGERALGO_H__

#include <vector>
#include "SWTriggerBase/SWTriggerTypes.h"
#include "SWTriggerBase/AlgoBase.h"
#include "FEMBeamTriggerTypes.h"

namespace trigger {

  namespace fememu {
    
    class FEMBeamTriggerAlgo : public trigger::AlgoBase {
      
    public:
      /// Default ctor
      FEMBeamTriggerAlgo();
      /// Config ctor
      FEMBeamTriggerAlgo(const FEMBeamTriggerConfig&);
      /// Default dtor
      ~FEMBeamTriggerAlgo(){}

      /// Summed (over channels) multiplicity vector getter
      const Waveform_t& Multiplicity() const;
      
      /// Summed (over channels) phmax vector getter
      const Waveform_t& PHMAX() const;
      
      /// Channel-wise disc 0 phase-diff vector getter
      const Waveform_t& PhaseDiffDisc0(const size_t) const;
      
      /// Channel-wise disc 3 phase-diff vector getter
      const Waveform_t& PhaseDiffDisc3(const size_t) const;
      
      /// Channel-wise multiplicity vector getter
      const Waveform_t& ChannelHitVector(const size_t) const;
      
      /// Channel-wise phmax vector getter
      const Waveform_t& ChannelPHMAXVector(const size_t) const;

    protected:

      /// AlgoBase::_Configure_() implementation
      void _Configure_();
      
      /// AlgoBase::_Process_() implementation
      const trigger::Result _Process_( const WaveformArray_t& );
      
    private:

      /// Function to define parameters in base class configuration object
      void DefineConfigParams();
      /// FEM configuration for generating FEM Beam trigger
      FEMBeamTriggerConfig _fememu_cfg;
      /// Computation variable: channel-wise phmax vector
      WaveformArray_t _chdiff;
      /// Computation variable: channel-wise multiplicity vector
      WaveformArray_t _chhit;
      /// Computation variable: disc. 0 diff vector
      WaveformArray_t _diff0_array;
      /// Computation variable: disc. 3 diff vector
      WaveformArray_t _diff3_array;
      /// Computation variable: phmax sum vector over channels
      Waveform_t _chdiff_sum;
      /// Computation variable: multiplicity sum vector over channels
      Waveform_t _chhit_sum;
      
    protected:
      inline bool debug   () const { return _fememu_cfg.fVerbosity <= kDEBUG;   }
      inline bool info    () const { return _fememu_cfg.fVerbosity <= kINFO;    }
      inline bool normal  () const { return _fememu_cfg.fVerbosity <= kNORMAL;  }
      inline bool warning () const { return _fememu_cfg.fVerbosity <= kWARNING; }
      inline bool error   () const { return _fememu_cfg.fVerbosity <= kERROR;   }
    };
  }
}

#endif
