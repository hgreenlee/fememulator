#ifndef __SWTRIGGERTYPES_H__
#define __SWTRIGGERTYPES_H__

#include <vector>
namespace trigger {

  /// Waveform
  typedef std::vector<unsigned short> Waveform_t;

  /// An array of waveforms
  typedef std::vector<Waveform_t> WaveformArray_t;

  enum ConfigVarType_t {
    kBOOL,            ///< bool
    kSTRING,          ///< std::string
    kINT,             ///< signed integer
    kDOUBLE,          ///< floating point
    kBOOL_ARRAY,      ///< std::vector<bool>
    kSTRING_ARRAY,    ///< std::vector<std::string>
    kINT_ARRAY,       ///< std::vector<int>
    kDOUBLE_ARRAY,    ///< std::vctor<double>
    kCONFIG_VAR_TYPE_MAX 
  };

}
#endif
