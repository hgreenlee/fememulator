#ifndef __FEMINTERFACE__
#define __FEMINTERFACE__

#include <vector>
#include <set>
#include <map>

namespace fememu {


  // Common Inputs for different Triggers
  class WaveformStore {

  public:
    WaveformStore() {};
    virtual ~WaveformStore() {};

  protected:
    std::set< int > channels;
    std::map< int, std::vector<short> > fWfms;

  public:
    typedef std::set< int >::iterator ChannelIter;
    ChannelIter chbegin() const { return channels.begin(); };
    ChannelIter chend() const { return channels.end(); };
    
    void add( const int channel, const std::vector<short>& wfm ) { 
      std::vector< short > mine = wfm;
      channels.insert( channel );
      fWfms[channel] = mine;
    };
    const std::vector<short>&  get( int ch ) const { return (*fWfms.find( ch )).second; };
    int size() const { return fWfms.size(); };
    
  };
  

  // Common output
  class TriggerOutput {

  public:
    TriggerOutput() {};
    virtual ~TriggerOutput() {};

    std::vector< short > vmaxdiff;
    std::vector< short > vmaxhit;

  };


}

#endif
