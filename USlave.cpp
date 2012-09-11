#include "WSlave.h"



namespace USlave {
  
  void begin()
  {
    Serial.begin(USB_SPEED);
  }
  
  
  void check()
  {
    if (Serial.available()) {
      LOGLN(">>> USB");
      Core::readLine(&Serial);
      _sendStatus();
      _close:
      // close the connection:
      Serial.flush();
      LOGLN("<<< USB");
    }
  }
  
  void _sendStatus()
  {
    
  }

}
