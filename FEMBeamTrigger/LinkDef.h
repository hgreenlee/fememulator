//
// cint script to generate libraries
// Declaire namespace & classes you defined
// #pragma statement: order matters! Google it ;)
//

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace trigger::fememu+;
#pragma link C++ class trigger::fememu::FEMBeamTriggerConfig+;
#pragma link C++ class trigger::fememu::FEMBeamTriggerAlgo+;
#pragma link C++ class trigger::fememu::FEMBeamTriggerAlgoFactory+;

#endif

