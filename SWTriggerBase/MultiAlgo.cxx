#ifndef MULTIALGO_CXX
#define MULTIALGO_CXX

#include "MultiAlgo.h"

namespace trigger {

  MultiAlgo::MultiAlgo()
    : _time_profile(0)
    , _process_count(0)
  {}

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
    _trigbit_to_index[trigbit].push_back( index );
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

  void MultiAlgo::Configure() {
    for ( auto it=(*this).begin(); it!=(*this).end(); it++) {
      (*it)->Configure();
    }
  }

  const ResultArray MultiAlgo::Process( unsigned int trigbit, const WaveformArray_t& wfms) {
    auto it=_trigbit_to_index.begin();

    ResultArray results;
    results.passedone = false;
    results.passedall = true;

    _watch.Start();
    
    for ( auto it=_trigbit_to_index.begin(); it!=_trigbit_to_index.end(); it++) {
      if ( trigbit & (*it).first ) {
	// loop through all algs that are to be run with this bit
	for ( auto it_alg=(*it).second.begin(); it_alg!=(*it).second.end(); it_alg++) {
	  Result res = (*this).at( (*it_alg) )->Process( trigbit, wfms );
	  //std::cout << "Ran " << (*this).at( (*it_alg) )->Name() << " hwbit=" << trigbit << " algo=" << res.pass_algo << " prescale=" << res.pass_prescale << std::endl;
	  results.passedone = (results.passedone || res.pass);
	  results.passedall = (results.passedall & res.pass);
	  results.emplace_back(res);
	}
      }
    }
    
    _time_profile  += _watch.WallTime();
    ++_process_count;

    return results;
  }

  double MultiAlgo::AverageProcessTime() const
  { return _time_profile / _process_count; }

}
#endif
