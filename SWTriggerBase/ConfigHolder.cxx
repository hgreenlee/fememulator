#ifndef CONFIGHOLDER_CXX
#define CONFIGHOLDER_CXX

#include "ConfigHolder.h"

namespace trigger {

  template<> std::map< std::string, bool        >& ConfigHolder::GetContainer() { return _bool_m;  }
  template<> std::map< std::string, std::string >& ConfigHolder::GetContainer() { return _str_m;   }
  template<> std::map< std::string, int         >& ConfigHolder::GetContainer() { return _int_m;   }
  template<> std::map< std::string, double      >& ConfigHolder::GetContainer() { return _float_m; }
  template<> std::map< std::string, std::vector< bool        > >& ConfigHolder::GetContainer() { return _bool_am;  }
  template<> std::map< std::string, std::vector< std::string > >& ConfigHolder::GetContainer() { return _str_am;   }
  template<> std::map< std::string, std::vector< int         > >& ConfigHolder::GetContainer() { return _int_am;   }
  template<> std::map< std::string, std::vector< double      > >& ConfigHolder::GetContainer() { return _float_am; }

}
#endif
