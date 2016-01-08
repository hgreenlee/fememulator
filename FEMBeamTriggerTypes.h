#ifndef __FEMBEAMTRIGGERTYPES__
#define  __FEMBEAMTRIGGERTYPES__

#include <vector>
#include <string>
#include <exception>

namespace fememu {

  /// Message level for an algorithm
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

  /**
     Beam FEM Trigger configuration parameter holder struct
   */
  struct FEMBeamTriggerConfig {
  public:

    MessageLevel_t fVerbosity; ///< Verbosity level

    short fMinReadoutTicks;  ///< Ignore waveform if has ticks less than this number. Used to separate cosmic windows and beam windows

    short fDiscr0precount;  ///< Precount for disc. 0 to fire
    short fDiscr0delay;     ///< Delay to form a diff vector
    short fDiscr0deadtime;  ///< Deadtime for disc. 0
    short fDiscr0threshold; ///< Disc. 0 threshold
    
    short fDiscr3width;     ///< Width for disc. 3
    short fDiscr3delay;     ///< Delay to form a diff vector
    short fDiscr3deadtime;  ///< Deadtime for disc. 3 (does not exist! FIX ME)
    short fDiscr3threshold; ///< Disc. 3 threshold

    short fDiscr3WindowStart; ///< tick where Disc3 fire-allowed window start
    short fDiscr3WindowSize;  ///< size of Disc3 fire-allowed window

    short fTriggerWindowStart; ///< start of PMT Trigger Window
    short fTriggerWindowSize;  ///< size of PMT Trigger Window in ticks

    short fTriggerThresMult;    ///< FEM beam trigger threshold (multiplicity)
    short fTriggerThresPHMAX;   ///< FEM beam trigger threshold (PHMAX sum)

    /// Default ctor
    FEMBeamTriggerConfig()
    {
      fVerbosity = kNORMAL;
      
      fDiscr0delay = 4;
      fDiscr0threshold = 5;
      fDiscr0deadtime = 6;
      fDiscr0precount = 3;
      
      fDiscr3delay = 4;
      fDiscr3threshold = 10;
      fDiscr3deadtime = 6;
      fDiscr3width = 6;

      fDiscr3WindowStart = 100;
      fDiscr3WindowSize  = 104;

      fTriggerWindowStart = 100;
      fTriggerWindowSize  = 104;

      fMinReadoutTicks = 500;
      fTriggerThresMult  = 4;
      fTriggerThresPHMAX = 80;
    }

  };

  /// Trigger decision result holder
  class FEMBeamTriggerOutput {
    
  public:
    
    /// Default ctor
    FEMBeamTriggerOutput(const size_t nwindows=0)
      : window_start (  0 )
      , window_end   (  0 )
      , vmaxdiff     (  0 )
      , vmaxhit      (  0 )
      , fire_time    ( -1 )
    {}

    /// Default dtor
    ~FEMBeamTriggerOutput() {}

    size_t window_start; ///< Window start tick
    size_t window_end;   ///< Window end   tick
    short  vmaxdiff;     ///< PHMAX sum
    short  vmaxhit;      ///< Multiplicity sum
    int    fire_time;    ///< Trigger decision times (-1 means not fired)
    
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
