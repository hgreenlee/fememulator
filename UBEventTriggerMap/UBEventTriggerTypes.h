

namespace trigger {


  typedef enum { 
    kUndefined,
    kBNBwFEMEmu,      // FEM Emulator Software trigger algo applied to BNB events
    kNUMIwFEMEmu,     // FEM Emulator Software trigger algo applied to NUMI events
    kEXTwFEMEmuBNB,   // FEM Emulator Software trigger algo applied to EXT events, BNB-sized window applied
    kEXTwFEMEmuNUMI,  // FEM Emulator Software trigger algo applied to EXT events, NUMI-sized window applied
    kBNBunbiased,     // Unbiased BNB event  (prescale may have been applied)
    kNUMIunbiased,    // Unbiased NUMI event (prescale may have been applied)
    kEXTunbiased,     // Unbiased EXT event  (prescale may have been applied)
    NumEventTrigs } UBEventTrigger_t;
 
  

}
