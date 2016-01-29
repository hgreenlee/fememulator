//
// cint script to generate libraries
// Declaire namespace & classes you defined
// #pragma statement: order matters! Google it ;)
//

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace trigger+;
#pragma link C++ class trigger::Result+;
#pragma link C++ class trigger::ResultArray+;
#pragma link C++ class trigger::AlgoFactory+;
#pragma link C++ class trigger::TriggerException+;
#pragma link C++ class std::map<std::string,bool>+;
#pragma link C++ class std::map<std::string,std::string>+;
#pragma link C++ class std::map<std::string,int>+;
#pragma link C++ class std::map<std::string,double>+;
#pragma link C++ class std::map<std::string,std::vector<bool> >+;
#pragma link C++ class std::map<std::string,std::vector<std::string> >+;
#pragma link C++ class std::map<std::string,std::vector<int> >+;
#pragma link C++ class std::map<std::string,std::vector<double> >+;

#pragma link C++ class trigger::ConfigHolder+;
#pragma link C++ class trigger::AlgoBase;
#pragma link C++ class trigger::PrescaleAlgo+;
#pragma link C++ class trigger::PrescaleAlgoFactory+;
#pragma link C++ class trigger::MultiAlgo+;
//#pragma link C++ function trigger::ConfigHolder::Set(const std::string& key, const int&, bool)+;
#endif






