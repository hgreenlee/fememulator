#ifndef __FEMBEAMTRIGGERTYPES__
#define  __FEMBEAMTRIGGERTYPES__

#include <vector>
#include <string>
namespace fememu {

  typedef std::vector<short> Waveform_t;
  typedef std::vector<Waveform_t> WaveformArray_t;

  /**
     Beam FEM Trigger configuration parameter holder struct
   */
  struct FEMBeamTriggerConfig {
  public:
    short fDiscr0delay; ///< Delay to form a diff vector
    short fDiscr3delay; ///< Delay to form a diff vector
    
    short fDiscr0threshold; ///< Disc. 0 threshold
    short fDiscr3threshold; ///< Disc. 3 threshold
 
    short fDiscr0precount; ///< Precount for disc. 0 to fire
    
    short fDiscr0deadtime; ///< Deadtime for disc. 0
    short fDiscr3deadtime; ///< Deadtime for disc. 3 (does not exist! FIX ME)
    
    short fDiscr0width; ///< Width for disc. 0... WHAT IS THIS?
    short fDiscr3width; ///< Width for disc. 3

    short fMinReadoutTicks; ///< Comment please!
    short fFrontBuffer;     ///< Comment please!
    short fWindowSize;      ///< Comment please!

    /// Default ctor
    FEMBeamTriggerConfig()
    {
      fDiscr0delay = fDiscr3delay = 3;

      fDiscr0threshold = 5;
      fDiscr3threshold = 10;

      fDiscr0precount = 3;

      fDiscr0deadtime = 6;
      fDiscr3deadtime = 6;

      fDiscr0width = 6;
      fDiscr3width = 6;

      fMinReadoutTicks = 0;
      fFrontBuffer = 20;
      fWindowSize = 6;
    }
    
  };

  /// Trigger decision result holder
  class FEMBeamTriggerOutput {
    
  public:
    
    /// Default ctor
    FEMBeamTriggerOutput(const size_t wf_size=1500)
      : vmaxdiff ( 1500, 0 )
      , vmaxhit  ( 1500, 0 )
    {}

    /// Default dtor
    ~FEMBeamTriggerOutput() {}
    
    std::vector< short  > vmaxdiff;    ///< PHMAX sum vector
    std::vector< short  > vmaxhit;     ///< Multiplicity sum vector
    std::vector< size_t > fire_time_v; ///< Trigger decision times

  };

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
