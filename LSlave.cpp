#include "LSlave.h"



namespace LSlave {
  
  void begin()
  {
    //_lcd.on();
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
    _lcd.write('@');
    LOGLN("display LCD");
  }
  
  
  void check()
  {
    if (_hasNewPulsedKey() && _key != KEYPAD_NONE) {
      LOGLN(">>> LCD");
      
      // wakeup
      if (_state != AWAKE) {
        _state = AWAKE;
        //_lcd.on();
      } else {
        
        // UP/DOWN: page select
        // LEFT/RIGHT: change value
        // SELECT: switch between INFO, MESSAGES, PULSES, DIGITALS
        switch (_key) {
          case KEYPAD_UP:
          if (_menuItem--) {
            _menuItem+= menu_len;
          }
          break;
          case KEYPAD_DOWN:
          if (++_menuItem == menu_len) {
            _menuItem = 0;
          }
          break;
          case KEYPAD_LEFT:
          _add((int8_t)+ANALOGSTEP);
          break;
          case KEYPAD_RIGHT:
          _add((int8_t)-ANALOGSTEP);
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
        
        //_lcd.home();
        _lcd.clear();
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
        //_lcd.setBacklight(LCD_BACKLIGHT_SLEEP);
        _state = SLEEPING;
        break;
        case SLEEPING:
        //_lcd.off();
        //_menuItem = 0;
        break;
      }
    }
  }
  
  
  void _printTitle(const prog_char* const label, const char type)
  {
    uint8_t i = 0;
    _lcd.setCursor(LCDPOSITION_TITLE_XY);
    /*
    while (i<LCDPOSITION_TITLE_LENGTH && pgm_read_byte_near(&label[i])) {
      _lcd.print((char)pgm_read_byte_near(&label[i]));
      i++;
    }
    */
    while (i<LCDPOSITION_TITLE_LENGTH && label[i]) {
      _lcd.print(label[i]);
      i++;
    }
    while (i++<LCDPOSITION_TITLE_LENGTH + LCDPOSITION_PAGE_OFFSET) {
      _lcd.print(' ');
    }
    _lcd.print(type);
    if (_menuItem<9) {
      _lcd.print(' ');
    }
    LOG("page #"); LOG(_menuItem+1); LOG('/'); LOGLN(menu_len);
    _lcd.print(_menuItem+1);
    _lcd.print(LCDCHAR_PAGESEPARATOR);
    _lcd.print(menu_len);
  }
  
  
  void _printInfo()
  {
    LOG("item I"); LOGLN(_menuItem);
    _printTitle(/*PSTR(*/"IP"/*)*/, 'I');
    _lcd.setCursor(0, 1);
    //#if USE_ETH
    _lcd.print(Ethernet.localIP());
    /*
    #else
    _lcd.print("--:--:--");
    #endif USE_ETH
    */
  }
  
  
  void _printMessage()
  {
    uint8_t index = _menuItem-NUMBEROFMENU_HOME;
    LOG("item M"); LOGLN(index);
    _printTitle(Core::messages[index].label, 'M');
    _lcd.setCursor(0, 1);
    _lcd.write("TODO"); // TODO
  }
  
  
  void _printPulse()
  {
    uint8_t index = _menuItem-NUMBEROFMENU_HOME-Core::messages_len;
    LOG("item P"); LOGLN(index);
    _printTitle(Core::getPulseLabelAtIndex(index), 'P');
    _lcd.setCursor(LCDPOSITION_ANALOG_X, 1);
    _lcd.write(LCDCHAR_LEFTBAR);
    uint8_t valueLeft = Core::getPulseValueAtIndex(index);
    for (uint8_t i=0; i<LCDPOSITION_BAR_LENGTH; i++) {
      if (valueLeft > 2* ANALOGSTEP) {
        valueLeft-= 2* ANALOGSTEP;
        _lcd.write(LCDCHAR_FULLBAR);
      } else if (valueLeft < ANALOGSTEP) {
        _lcd.write(LCDCHAR_VOIDBAR);
      } else {
        valueLeft = 0;
        _lcd.write(LCDCHAR_HALFBAR);
      }
    }
    _lcd.write(LCDCHAR_RIGHTBAR);
    for (uint8_t i=0; i<LCDPOSITION_ANALOG_OFFSET; i++) {
      _lcd.print(' ');
    }
    _lcd.print(Core::getPulseValueAtIndex(index));
  }
  
  
  void _printDigital()
  {
    uint8_t index = _menuItem-NUMBEROFMENU_HOME-Core::messages_len-Core::pulses_len;
    char on, off;
    LOG("item D"); LOGLN(index);
    _printTitle(Core::getDigitalLabelAtIndex(index), 'D');
    _lcd.setCursor(LCDPOSITION_DIGITAL_X, 1);
    if (Core::getDigitalValueAtIndex(index)) {
      on = '*';
      off = ' ';
    } else {
      on = ' ';
      off = '*';
    }
    _lcd.write(on);
    _lcd.write("ON");
    _lcd.write(on);
    for (uint8_t i=0; i<LCDPOSITION_DIGITAL_INSET; i++) {
      _lcd.print(' ');
    }
    _lcd.write(off);
    _lcd.write("OFF");
    _lcd.write(off);
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
  
  
  void _add(const int8_t delta)
  {
  }

}
