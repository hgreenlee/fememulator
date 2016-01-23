#ifndef __PRESCALE_H__
#define __PRESCALE_H__

#include <vector>
#include "SWTriggerTypes.h"
#include "AlgoBase.h"
#include "AlgoFactory.h"

namespace trigger {

  class PrescaleAlgo : public trigger::AlgoBase {
    
  public:
    /// Default ctor
    PrescaleAlgo(const std::string name="PrescaleAlgoInstance");
    /// Default dtor
    ~PrescaleAlgo(){}
    
  protected:

    /// AlgoBase::_Configure_() implementation
    void _Configure_() {};
    
    /// AlgoBase::_Process_() implementation
    const trigger::Result _Process_( unsigned int triggerbit, const WaveformArray_t& );
    
  private:
    
  };

  class PrescaleAlgoFactory : public ::trigger::AlgoFactory {
    public:
      PrescaleAlgoFactory() {
	AlgoBase::registerConcreteFactory( "PrescaleAlgo",this );
      };
      virtual ~PrescaleAlgoFactory() {};
      virtual AlgoBase* create(std::string algoinstance_name) {
	return new PrescaleAlgo(algoinstance_name);
      };
  };

  static PrescaleAlgoFactory __global_PrescaleAlgoFactory__;
  
}

#endif
