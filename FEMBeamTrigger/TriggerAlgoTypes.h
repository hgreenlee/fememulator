#ifndef __TRIGGER_ALGO_TYPES__
#define __TRIGGER_ALGO_TYPES__

#include <vector>
#include <string>
#include <exception>

namespace uboonetrigger {

  enum MessageLevel_t {
    kDEBUG,
    kINFO,
    kNORMAL,
    kWARNING,
    kERROR,
    kCRITICAL
  };

  typedef std::vector<short> Waveform_t;
  typedef std::vector<Waveform_t> WaveformArray_t;

  /// Exception class for FEMemulator
  class EmulatorException : public std::exception {
  public:
    /// default ctor 
    EmulatorException(std::string msg="") : std::exception()
    {
      _msg  = "\033[93m";
      _msg += msg;
      _msg += "\033[00m";
    }
    /// default dtor
    virtual ~EmulatorException() throw(){};
    /// override msg passer
    virtual const char* what() const throw()
    { return _msg.c_str(); }
    
  private:
    std::string _msg;
  };
  
}

#endif
