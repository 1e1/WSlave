#include "LSlave.h"



namespace LSlave {
  
  void begin()
  {
    _lcd.on();
    _lcd.begin(LCD_WIDTH, LCD_HEIGHT);
    _lcd.home();
    LOGLN("display LCD");
  }
  
  
  void check()
  {
    if (_hasNewPulsedKey() && _key != KEYPAD_NONE) {
      LOGLN(">>> LCD");
      _lcd.on();
      // UP/DOWN: page select
      // LEFT/RIGHT: change value
      // SELECT: switch between INFO, MESSAGES, PULSES, DIGITALS
      switch (_key) {
        case KEYPAD_UP:
        if (++_menuItem == menu_len) {
          _menuItem = 0;
        }
        break;
        case KEYPAD_DOWN:
        if (!_menuItem--) {
          _menuItem+= menu_len;
        }
        break;
        case KEYPAD_LEFT:
        _set(_menuItem, +15);
        break;
        case KEYPAD_RIGHT:
        _set(_menuItem, -15);
        break;
        case KEYPAD_SELECT:
        // jump section
        if (_menuItem < NUMBEROFMENU_HOME) {
          _menuItem = NUMBEROFMENU_HOME;
        } else if (_menuItem < NUMBEROFMENU_HOME + Core::messages_len) {
          _menuItem = NUMBEROFMENU_HOME + Core::messages_len;
        } else if (_menuItem < NUMBEROFMENU_HOME + Core::messages_len + Core::pulses_len) {
          _menuItem = NUMBEROFMENU_HOME + Core::messages_len + Core::pulses_len;
        } else { // if (_menuItem < NUMBEROFMENU_HOME + Core::messages_len + Core::pulses_len + Core::digitals_len)
          _menuItem = 0;
        }
        break;
      }
      // update display
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
  
  
  const Key _getKey(const uint8_t menuItem, const uint8_t value)
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
  
  
  void _set()
  {
  }

}
