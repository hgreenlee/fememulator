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
    FEMBeamTriggerAlgo(const FEMBeamTriggerConfig& cfg);
    /// Default dtor
    ~FEMBeamTriggerAlgo(){}

    /// Executor function
    FEMBeamTriggerOutput Emulate( const WaveformArray_t& );

    /// Verbosity level
    MessageLevel_t _verbosity;
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
    inline bool debug   () const { return _verbosity <= kDEBUG;   }
    inline bool info    () const { return _verbosity <= kINFO;    }
    inline bool normal  () const { return _verbosity <= kNORMAL;  }
    inline bool warning () const { return _verbosity <= kWARNING; }
    inline bool error   () const { return _verbosity <= kERROR;   }
  };

}

#endif
