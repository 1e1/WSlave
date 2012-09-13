#include "LSlave.h"



namespace LSlave {
  
  void begin()
  {
    _lcd.begin(LCD_WIDTH, LCD_HEIGHT);
  #if LCD_BLPIN
    pinMode(LCD_BLPIN, OUTPUT);
    digitalWrite(LCD_BLPIN, LIGHTON);
  #endif
    LOGLN("display LCD");
  }
  
  
  void check()
  {
    if (_hasNewPulsedKey() && _key != KEYPAD_NONE) {
      LOGLN(">>> LCD");
      digitalWrite(LCD_BLPIN, LIGHTON);
      LOGLN("<<< LCD");
    }
  }
  
  
  void uncheck()
  {
  }
  
  
  void shutdown()
  {
  }
  
  
  const boolean _hasNewPulsedKey()
  {
    Key currentKey = _getKey();
    if (_key != currentKey) {
      _key = currentKey;
      return true;
    }
    return false;
  }
  
  
  const Key _getKey()
  {
    int input = analogRead(LCD_ABTN);
    if (input > KEYPAD_MAXVALUE_SELECT) return KEYPAD_NONE  ;
    if (input > KEYPAD_MAXVALUE_LEFT  ) return KEYPAD_SELECT;
    if (input > KEYPAD_MAXVALUE_DOWN  ) return KEYPAD_LEFT  ;
    if (input > KEYPAD_MAXVALUE_UP    ) return KEYPAD_DOWN  ;
    if (input > KEYPAD_MAXVALUE_RIGHT ) return KEYPAD_UP    ;
    return KEYPAD_RIGHT ;
    /*
    if (input < KEYPAD_MAXVALUE_RIGHT ) return KEYPAD_RIGHT ;
    if (input < KEYPAD_MAXVALUE_UP    ) return KEYPAD_UP    ;
    if (input < KEYPAD_MAXVALUE_DOWN  ) return KEYPAD_DOWN  ;
    if (input < KEYPAD_MAXVALUE_LEFT  ) return KEYPAD_LEFT  ;
    if (input < KEYPAD_MAXVALUE_SELECT) return KEYPAD_SELECT;
    return KEYPAD_NONE;
    */
  }

}
