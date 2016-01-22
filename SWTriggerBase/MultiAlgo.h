/**
 * \file MultiAlgo.h
 *
 * \ingroup SWTriggerBase
 * 
 * \brief Class def header for Algorithm Container class
 *
 * @author kazuhiro,twongjirad
 */

/** \addtogroup SWTriggerBase

    @{*/
#ifndef MULTIALGO_H
#define MULTIALGO_H

#include <iostream>
#include <string>
#include <map>

#include "AlgoBase.h"
#include "ConfigHolder.h"
#include "Result.h"
#include "Watch.h"

namespace trigger {
  /**
     \class MultiAlgo
     This is a container class of SWtrigger Algorithms
     It assumes to make a trigger decision based on trigger::WaveformArray_t. \n
     One can define any number of algorithms to be applied for a given hardware trigger bit. \n
  */
  class MultiAlgo : std::vector<AlgoBase*> {
    
  public:
    
    /**
       Constructor: this is where an algorithm should \n
       set configuration parameter names.
    */
    MultiAlgo() {};
    
    /// Default destructor
    virtual ~MultiAlgo();
    
    /// declare an algo in the list
    void declareAlgo( unsigned int trigbit, std::string algotype_name, std::string algoinstance_name );

    /// Get config for a given algo
    ConfigHolder& GetConfig( std::string algoinstance_name );

    AlgoBase& GetAlgo( std::string algoinstance_name );

    /// Run trigger algorithms
    const Results Process( unsigned int trigbit, const WaveformArray_t&);

  protected:

    // map of algos assigned to each bit
    std::map< unsigned int, int > _trigbit_to_index; // [bit,index]
    std::map< std::string, int > _name_to_index;

  };
}
#endif
/** @} */ // end of doxygen group 

