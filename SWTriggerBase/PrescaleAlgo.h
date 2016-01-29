#ifndef __PRESCALE_H__
#define __PRESCALE_H__

#include <vector>
#include "SWTriggerTypes.h"
#include "AlgoFactory.h"
#include "AlgoBase.h"

namespace trigger {

  class PrescaleAlgo : public AlgoBase {
    
  public:
    /// Default ctor
    PrescaleAlgo(const std::string name="PrescaleAlgoInstance");
    /// Default dtor
    ~PrescaleAlgo(){}
    
  protected:

    /// AlgoBase::_Configure_() implementation
    void _Configure_() {};
    
    /// AlgoBase::_Process_() implementation
    const Result _Process_( unsigned int triggerbit, const WaveformArray_t& );
    
  private:
    
  };

  class PrescaleAlgoFactory : public AlgoFactoryBase {
    public:
    PrescaleAlgoFactory(){ AlgoFactory::get()->add_factory("PrescaleAlgo",this); }
    ~PrescaleAlgoFactory(){}
    AlgoBase* create(std::string algoinstance_name) 
    {return new PrescaleAlgo(algoinstance_name);}
  };

}

#endif
