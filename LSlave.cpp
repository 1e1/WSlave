#include "LSlave.h"



namespace LSlave {
  
  void begin()
  {
    _lcd.begin(LCD_WIDTH, LCD_HEIGHT);
  #if LCD_BLPIN
    pinMode(LCD_BLPIN, OUTPUT);
    digitalWrite(LCD_BLPIN, HIGH);
  #endif
    LOGLN("display LCD");
  }
  
  
  void check()
  {
    if (Serial.available()) {
      LOGLN(">>> LCD");
    }
  }
  
  
  void uncheck()
  {
    LOGLN("<<< LCD");
  }

}
