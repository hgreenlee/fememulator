/**
 * \file TriggerException.h
 *
 * \ingroup SWTriggerBase
 * 
 * \brief Class def header for a class TriggerException
 *
 * @author kazuhiro
 */

/** \addtogroup SWTriggerBase

    @{*/
#ifndef __TRIGGER_EXCEPTION_H__
#define __TRIGGER_EXCEPTION_H__

#include <iostream>
#include <exception>
namespace trigger {

  /// Exception class for software trigger
  class TriggerException : public std::exception {
  public:
    /// default ctor
    TriggerException(std::string msg="") : std::exception()
    {
      _msg  = "\033[93m";
      _msg += msg;
      _msg += "\033[00m";
    }
    /// default dtor
    virtual ~TriggerException() throw(){};
    /// override msg passer
    virtual const char* what() const throw()
    { return _msg.c_str(); }
    
  private:
    std::string _msg;
  };
}
#endif
/** @} */ // end of doxygen group 

