#ifndef __ALGOFACTORY__
#define __ALGOFACTORY__

#include "TriggerException.h"
#include <string>
#include <map>

namespace trigger {
  
  class AlgoBase;
  
  class AlgoFactoryBase {
  public:
    AlgoFactoryBase(){}
    virtual ~AlgoFactoryBase(){}
    virtual AlgoBase* create(std::string instance_name) = 0;
  };

  class AlgoFactory {
  public:
    AlgoFactory(){}
    ~AlgoFactory(){}
    static AlgoFactory* get() {
      if(!_me) _me = new AlgoFactory;
      return _me;
    }
    AlgoBase* create(std::string type_name,std::string instance_name) {
      auto iter = _factory.find(type_name);
      if(iter == _factory.end()) {
	std::cerr << "\033[93mUnknown trigger type requested...\033[00m" << std::endl;
	return nullptr;
      }
      return (*iter).second->create(instance_name);
    }

    void add_factory(std::string type_name,AlgoFactoryBase* factory) {
      if(!factory) return;
      auto iter = _factory.find(type_name);
      if(iter != _factory.end()) {
	std::cerr<<"\033[93mDuplicate factory registration for " << type_name << std::endl;
	throw TriggerException("Duplicate factory registration...");
      }
      _factory[type_name]=factory;
      //std::cout<<"Registered: "<<type_name<<std::endl;
    }

  private:

    static AlgoFactory* _me;
    std::map<std::string,::trigger::AlgoFactoryBase*> _factory;
  };
  
}

#endif
