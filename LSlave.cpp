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
      LOGLN("<<< LCD");
    }
  }
  
  
  void uncheck()
  {
  }
  
  
  const Key getKey()
  {
    int input = analogRead(LCD_ABTN);
    if (input < KEYPAD_MAXVALUE_RIGHT ) return KEYPAD_RIGHT ;
    if (input < KEYPAD_MAXVALUE_UP    ) return KEYPAD_UP    ;
    if (input < KEYPAD_MAXVALUE_DOWN  ) return KEYPAD_DOWN  ;
    if (input < KEYPAD_MAXVALUE_LEFT  ) return KEYPAD_LEFT  ;
    if (input < KEYPAD_MAXVALUE_SELECT) return KEYPAD_SELECT;
    return KEYPAD_NONE;
  }

}
