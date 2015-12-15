#ifndef __FEMBEAMTRIGGERALGO__
#define  __FEMBEAMTRIGGERALGO__

#include <vector>
#include "FEMBeamTriggerTypes.h"

namespace fememu {

  class FEMBeamTriggerAlgo {
  public:
    /// Default ctor
    FEMBeamTriggerAlgo();
    /// Config ctor
    FEMBeamTriggerAlgo(const FEMBeamTriggerConfig&);
    /// Default dtor
    ~FEMBeamTriggerAlgo(){}

    /// Executor function
    const FEMBeamTriggerOutput Emulate( const WaveformArray_t& );

    /// FEM configuration for generating FEM Beam trigger
    FEMBeamTriggerConfig _cfg;

  private:
    /// Computation variable: channel-wise phmax vector
    WaveformArray_t _chdiff;
    /// Computation variable: channel-wise multiplicity vector
    WaveformArray_t _chhit;
    /// Computation variable: disc. 0 diff vector
    Waveform_t _diff0;
    /// Computation variable: disc. 3 diff vector
    Waveform_t _diff3;

  protected:
    inline bool debug   () const { return _cfg.fVerbosity <= kDEBUG;   }
    inline bool info    () const { return _cfg.fVerbosity <= kINFO;    }
    inline bool normal  () const { return _cfg.fVerbosity <= kNORMAL;  }
    inline bool warning () const { return _cfg.fVerbosity <= kWARNING; }
    inline bool error   () const { return _cfg.fVerbosity <= kERROR;   }
  };

}

#endif
