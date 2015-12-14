//
// cint script to generate libraries
// Declaire namespace & classes you defined
// #pragma statement: order matters! Google it ;)
//

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace fememu;
#pragma link C++ function fememu::basicTrigger;
#pragma link C++ class fememu::TriggerOutput+;
#pragma link C++ class fememu::WaveformStore+;
#pragma link C++ class fememu::BasicTriggerConfig+;
#endif

