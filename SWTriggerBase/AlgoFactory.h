#ifndef __ALGOFACTORY__
#define __ALGOFACTORY__

namespace trigger {
  
  class AlgoBase;
  
  class AlgoFactory {
  public:
    virtual AlgoBase* create(std::string instance_name) = 0;
  };
  
}

#endif
