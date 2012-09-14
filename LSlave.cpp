#include "LSlave.h"



namespace LSlave {
  
  void begin()
  {
    // _lcd.on();
    _lcd.begin(LCD_WIDTH, LCD_HEIGHT);
    {
      const uint8_t symbol[8] = LCDCHAR_VOIDBAR_CONTENT;
      _lcd.createChar(LCDCHAR_VOIDBAR, symbol);
    }{
      const uint8_t symbol[8] = LCDCHAR_HALFBAR_CONTENT;
      _lcd.createChar(LCDCHAR_HALFBAR, symbol);
    }{
      const uint8_t symbol[8] = LCDCHAR_FULLBAR_CONTENT;
      _lcd.createChar(LCDCHAR_FULLBAR, symbol);
    }{
      const uint8_t symbol[8] = LCDCHAR_LEFTBAR_CONTENT;
      _lcd.createChar(LCDCHAR_LEFTBAR, symbol);
    }{
      const uint8_t symbol[8] = LCDCHAR_RIGHTBAR_CONTENT;
      _lcd.createChar(LCDCHAR_RIGHTBAR, symbol);
    }{
      const uint8_t symbol[8] = LCDCHAR_NOLEFTBAR_CONTENT;
      _lcd.createChar(LCDCHAR_NOLEFTBAR, symbol);
    }{
      const uint8_t symbol[8] = LCDCHAR_NORIGHTBAR_CONTENT;
      _lcd.createChar(LCDCHAR_NORIGHTBAR, symbol);
    }
    // lcd.write(LCDCHAR_FULLBAR);
    _lcd.home();
    LOGLN("display LCD");
  }
  
  
  void check()
  {
    if (_hasNewPulsedKey() && _key != KEYPAD_NONE) {
      LOGLN(">>> LCD");
      
      // wakeup
      if (_state != AWAKE) {
        _state = AWAKE;
        _lcd.on();
      } else {
        
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
          _add(+ANALOGSTEP);
          break;
          case KEYPAD_RIGHT:
          _add(-ANALOGSTEP);
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
        
        if (_menuItem < NUMBEROFMENU_HOME) {
          // display info (_menuItem)
          _printInfo();
        } else if (_menuItem < NUMBEROFMENU_HOME + Core::messages_len) {
          // display info (_menuItem - NUMBEROFMENU_HOME)
          _printMessage();
        } else if (_menuItem < NUMBEROFMENU_HOME + Core::messages_len + Core::pulses_len) {
          // display info (_menuItem - NUMBEROFMENU_HOME - Core::messages_len)
          _printPulse();
        } else { // if (_menuItem < NUMBEROFMENU_HOME + Core::messages_len + Core::pulses_len + Core::digitals_len)
          // display info (_menuItem - NUMBEROFMENU_HOME - Core::messages_len - Core::pulses_len)
          _printDigital();
        }
        
      }
      // update display
      LOG("display menu #"); LOGLN(_menuItem);
      
      
      LOGLN("<<< LCD");
    }
  }
  
  
  void uncheck()
  {
  }
  
  
  void shutdown()
  {
    if (_state != SHUTDOWN) {
      switch (_state) {
        case AWAKE:
        _lcd.setBacklight(LCD_BACKLIGHT_SLEEP);
        _state = SLEEPING;
        break;
        case SLEEPING:
        _lcd.off();
        _state = SHUTDOWN;
        //_menuItem = 0;
        break;
      }
    }
  }
  
  
  void _printTitle(const char *label, const char type)
  {
    uint8_t i = 0;
    while (i<LCDPOSITION_TITLE_LENGTH && label[i]) {
      _lcd.print(label[i]);
      i++;
    }
    while (i++<LCDPOSITION_TITLE_LENGTH + LCDPOSITION_PAGE_OFFSET) {
      _lcd.print(' ');
    }
    if (_menuItem<9) {
      _lcd.print(' ');
    }
    _lcd.print(_menuItem+1);
    _lcd.print(LCDCHAR_PAGESEPARATOR);
    _lcd.print(Core::total_len);
  }
  
  
  void _printInfo()
  {
    _printTitle("IP", 'I');
  }
  
  
  void _printMessage()
  {
    
  }
  
  
  void _printPulse()
  {
    
  }
  
  
  void _printDigital()
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
