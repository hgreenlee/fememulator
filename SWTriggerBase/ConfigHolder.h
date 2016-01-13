/**
 * \file ConfigHolder.h
 *
 * \ingroup SWTriggerBase
 * 
 * \brief Class def header for a class ConfigHolder
 *
 * @author kazuhiro
 */

/** \addtogroup SWTriggerBase

    @{*/
#ifndef CONFIGHOLDER_H
#define CONFIGHOLDER_H

#include <iostream>
#include "SWTriggerTypes.h"
#include "TriggerException.h"
//#include "UtilFunc.h"
#include <map>
#include <vector>
namespace trigger {
  /**
     \class ConfigHolder
     User defined class ConfigHolder ... these comments are used to generate
     doxygen documentation!
  */
  class ConfigHolder{
    
  public:
    
    /// Default constructor
    ConfigHolder(const std::string name = "noname") : _name(name) {}
    
    /// Default destructor
    ~ConfigHolder(){}

    const std::string& Name() const { return _name; }

    template <class T>
    std::vector<std::string> ListKeys()
    {
      auto const& holder = GetContainer<T>();
      std::vector<std::string> result;
      result.reserve(holder.size());
      for(auto const& key_value : holder) result.push_back(key_value.first);
      return result;
    }

    template <class T>
    const T& Get(const std::string& key)
    {
      auto const& holder = GetContainer<T>();
      auto iter = holder.find(key);
      if(iter == holder.end()) {
	std::string msg = "Key \"" + key + "\" not found!";
	throw TriggerException(msg);
      }
      return (*iter).second;
    }

    template <class T>
    void Set(const std::string& key, const T& value, bool over_write=false)
    {
      auto& holder = GetContainer<T>();
      if(!over_write && holder.find(key) != holder.end()) {
	std::string msg = "Key \"" + key + "\" already exists!";
	throw TriggerException(msg);
      }
      holder[key] = value;
    }
    
  private:

    template <class T>
    std::map<std::string, T>& GetContainer();

    std::string _name;

    std::map< std::string, bool        > _bool_m;
    std::map< std::string, std::string > _str_m;
    std::map< std::string, int         > _int_m;
    std::map< std::string, double      > _float_m;

    std::map< std::string, std::vector<bool>        > _bool_am;
    std::map< std::string, std::vector<std::string> > _str_am;
    std::map< std::string, std::vector<int>         > _int_am;
    std::map< std::string, std::vector<double>      > _float_am;
    
  };

  template<> std::map< std::string, bool        >& ConfigHolder::GetContainer();
  template<> std::map< std::string, std::string >& ConfigHolder::GetContainer();	
  template<> std::map< std::string, int         >& ConfigHolder::GetContainer();	
  template<> std::map< std::string, double      >& ConfigHolder::GetContainer();
  template<> std::map< std::string, std::vector< bool        > >& ConfigHolder::GetContainer();
  template<> std::map< std::string, std::vector< std::string > >& ConfigHolder::GetContainer();
  template<> std::map< std::string, std::vector< int         > >& ConfigHolder::GetContainer();
  template<> std::map< std::string, std::vector< double      > >& ConfigHolder::GetContainer();

}

#endif
/** @} */ // end of doxygen group 

