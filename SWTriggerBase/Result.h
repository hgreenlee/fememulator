/**
 * \file Result.h
 *
 * \ingroup Base
 * 
 * \brief Class def header for a class Result
 *
 * @author kazuhiro
 */

/** \addtogroup Base

    @{*/
#ifndef __TRIGGER_RESULT_H__
#define __TRIGGER_RESULT_H__

#include <iostream>

namespace trigger {
  /**
     \class Result
     User defined class Result ... these comments are used to generate
     doxygen documentation!
  */
  class Result{
    
  public:
    
    /// Default constructor
    Result()
      : pass         (  0 )
      , amplitude    (  0 )
      , multiplicity (  0 )
      , time         ( -1 )
    {}
    
    /// Default destructor
    ~Result(){}

    bool  pass;         ///< Triggered yes/no
    short amplitude;    ///< PHMAX sum
    short multiplicity; ///< Multiplicity sum
    int   time;         ///< Trigger decision times (-1 means not fired)
    
  };
}
  
#endif
/** @} */ // end of doxygen group 

