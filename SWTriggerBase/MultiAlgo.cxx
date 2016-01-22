#ifndef MULTIALGO_CXX
#define MULTIALGO_CXX

#include "MultiAlgo.h"

namespace trigger {

  MultiAlgo::~MultiAlgo() {
    for (auto it=(*this).begin(); it!=(*this).end(); it++) {
      delete (*it);
    }
  }

  void MultiAlgo::declareAlgo( unsigned int trigbit, std::string algotype_name, std::string algoinstance_name ) {
    auto it = _name_to_index.find( algoinstance_name );
    if ( it!=_name_to_index.end() ) {
      char oops[500];
      sprintf(oops,"Already declared a trigger with the name '%s'. Duplicate instance names are forbidden!",algoinstance_name.c_str());
      throw TriggerException(oops);
    }
    (*this).push_back( AlgoBase::create( algotype_name, algoinstance_name ) );
    int index = (*this).size()-1;
    _trigbit_to_index[trigbit] = index;
    _name_to_index[algoinstance_name] = index;
  }

  AlgoBase& MultiAlgo::GetAlgo( std::string algoinstance_name ) {
    auto it = _name_to_index.find( algoinstance_name );
    if ( it==_name_to_index.end() ) {
      char oops[500];
      sprintf(oops,"could not find algo with instance name '%s'",algoinstance_name.c_str());
      throw TriggerException(oops);
    }
    return *(*this).at( (*it).second );
  }

  ConfigHolder& MultiAlgo::GetConfig( std::string instance_name ) {
    return GetAlgo( instance_name ).GetConfig();
  }

  const ResultArray MultiAlgo::Process( unsigned int trigbit, const WaveformArray_t& wfms) {
    auto it=_trigbit_to_index.begin();

    ResultArray results;
    results.passedone = false;
    results.passedall = true;

    for ( auto it=_trigbit_to_index.begin(); it!=_trigbit_to_index.end(); it++) {
      
      if ( trigbit & (*it).first ) {
	Result res = (*this).at( (*it).second )->Process( trigbit, wfms );
	results.passedone = results.passedone || res.pass;
	results.passedall = results.passedall && res.pass;
	results.emplace_back(res);
      }

    }

    return results;
  }

}
#endif
