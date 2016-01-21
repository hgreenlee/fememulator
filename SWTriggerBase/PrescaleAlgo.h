#ifndef __PRESCALE_H__
#define __PRESCALE_H__

#include <vector>
#include "SWTriggerTypes.h"
#include "AlgoBase.h"

namespace trigger {

  class PrescaleAlgo : public trigger::AlgoBase {
    
  public:
    /// Default ctor
    PrescaleAlgo(const std::string name="PrescaleAlgo");
    /// Default dtor
    ~PrescaleAlgo(){}
    
  protected:

    /// AlgoBase::_Configure_() implementation
    void _Configure_();
    
    /// AlgoBase::_Process_() implementation
    const trigger::Result _Process_( const WaveformArray_t& );
    
  private:
    
    /// Function to define parameters in base class configuration object
    void DefineConfigParams();
    
    /// Prescale factor
    size_t _prescale;
    
    /// Internal counter
    size_t _process_ctr;
    
  };
}

#endif
