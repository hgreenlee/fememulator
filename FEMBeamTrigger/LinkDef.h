//
// cint script to generate libraries
// Declaire namespace & classes you defined
// #pragma statement: order matters! Google it ;)
//

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

/* #pragma link C++ namespace fememu+; */
/* #pragma link C++ class fememu::FEMBeamTriggerConfig+; */
/* #pragma link C++ class fememu::FEMBeamTriggerOutput+; */
/* #pragma link C++ class fememu::FEMBeamTriggerAlgo+; */

#pragma link C++ namespace uboonetrigger+;
/* #pragma link C++ class uboonetrigger::TriggerConfigBase+; */
/* #pragma link C++ class uboonetrigger::TriggerConfigBase+; */
#pragma link C++ class uboonetrigger::TriggerOutputBase+;

#pragma link C++ class uboonetrigger::FEMBeamTriggerConfig+;
#pragma link C++ class uboonetrigger::FEMBeamTriggerOutput+;
//#pragma link C++ class uboonetrigger::FEMBeamTriggerAlgo+;


#endif

