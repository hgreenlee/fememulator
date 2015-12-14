#ifndef __FEMBEAMTRIGGERALGO__
#define  __FEMBEAMTRIGGERALGO__

#include <vector>
#include "FEMBeamTriggerTypes.h"

namespace fememu {

  class FEMBeamTriggerAlgo {
  public:
    /// Default ctor
    FEMBeamTriggerAlgo() {}
    /// Default dtor
    ~FEMBeamTriggerAlgo(){}

    /// Executor function
    const FEMBeamTriggerOutput& Emulate( const WaveformArray_t& );

    /// Config resetter (resets output holder as well)
    void Reset(const FEMBeamTriggerConfig& );

  private:
    /// FEM configuration for generating FEM Beam trigger
    FEMBeamTriggerConfig _cfg;
    /// Output trigger decision holder
    FEMBeamTriggerOutput _trigger;
    /// Computation variable: channel-wise phmax vector
    WaveformArray_t _chdiff;
    /// Computation variable: channel-wise multiplicity vector
    WaveformArray_t _chhit;
    /// Computation variable: disc. 0 diff vector
    Waveform_t _diff0;
    /// Computation variable: disc. 3 diff vector
    Waveform_t _diff3;
  };

}

#endif
