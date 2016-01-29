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
#include <vector>

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
    MultiAlgo();
    
    /// Default destructor
    virtual ~MultiAlgo();
    
    /// declare an algo in the list
    void declareAlgo( unsigned int trigbit, std::string algotype_name, std::string algoinstance_name );

    /// Get config for a given algo
    ConfigHolder& GetConfig( std::string algoinstance_name );

    /// Get an Algo by the instance name
    AlgoBase& GetAlgo( std::string algoinstance_name );

    /// Configure All Algos
    void Configure();

    /// Run trigger algorithms
    const ResultArray Process( unsigned int trigbit, const WaveformArray_t&);

    /// Process time getter
    double AverageProcessTime() const;

  protected:

    // map of algos assigned to each bit
    std::map< unsigned int, std::vector<int> > _trigbit_to_index; // [bit,indices]
    std::map< std::string, int > _name_to_index;
    /// Process time using trigger::Watch
    double _time_profile;
    /// Process counter
    double _process_count;
    /// Watch for simple profiling
    ::trigger::Watch _watch;

  };
}
#endif
/** @} */ // end of doxygen group 

