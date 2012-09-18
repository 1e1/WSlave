#include "LSlave2.h"




/***********************************************************
 *                       PROPERTIES                        *
 **********************************************************/




LiquidCrystal LSlave2::_lcd(LCD_PINS/*, LCD_BLPIN, LCD_BLPOLARITY*/);
//char LSlave2::_lcdLines[LCD_HEIGHT][LCD_WIDTH];

LSlave2::Key LSlave2::_key      = KEYPAD_NONE;
LSlave2::State LSlave2::_state  = SLEEPING;
uint8_t LSlave2::_menuItem      = 0;




/***********************************************************
 *                         PUBLIC                          *
 **********************************************************/




void LSlave2::begin()
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


void LSlave2::check()
{
  if (hasNewPulsedKey() && _key != KEYPAD_NONE) {
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
        add((int8_t)+ANALOGSTEP);
        break;
        case KEYPAD_RIGHT:
        add((int8_t)-ANALOGSTEP);
        break;
        case KEYPAD_SELECT:
        // jump section
        if (_menuItem < NUMBEROFMENU_HOME) {
          _menuItem = NUMBEROFMENU_HOME;
        } else if (_menuItem < NUMBEROFMENU_HOME + STATIC_MESSAGES_LEN) {
          _menuItem = NUMBEROFMENU_HOME + STATIC_MESSAGES_LEN;
        } else if (_menuItem < NUMBEROFMENU_HOME + STATIC_MESSAGES_LEN + STATIC_PULSES_LEN) {
          _menuItem = NUMBEROFMENU_HOME + STATIC_MESSAGES_LEN + STATIC_PULSES_LEN;
        } else { // if (_menuItem < NUMBEROFMENU_HOME + STATIC_MESSAGES_LEN + STATIC_PULSES_LEN + STATIC_DIGITALS_LEN)
          _menuItem = 0;
        }
        break;
      }
      
      //_lcd.home();
      _lcd.clear();
      if (_menuItem < NUMBEROFMENU_HOME) {
        // display info (_menuItem)
        printInfo();
      } else if (_menuItem < NUMBEROFMENU_HOME + STATIC_MESSAGES_LEN) {
        // display info (_menuItem - NUMBEROFMENU_HOME)
        printMessage();
      } else if (_menuItem < NUMBEROFMENU_HOME + STATIC_MESSAGES_LEN + STATIC_PULSES_LEN) {
        // display info (_menuItem - NUMBEROFMENU_HOME - STATIC_MESSAGES_LEN)
        printPulse();
      } else { // if (_menuItem < NUMBEROFMENU_HOME + STATIC_MESSAGES_LEN + STATIC_PULSES_LEN + STATIC_DIGITALS_LEN)
        // display info (_menuItem - NUMBEROFMENU_HOME - STATIC_MESSAGES_LEN - STATIC_PULSES_LEN)
        printDigital();
      }
      
    }
    
    LOGLN("<<< LCD");
  }
}


void LSlave2::shutdown()
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




/***********************************************************
 *                        PROTECTED                        *
 **********************************************************/




void LSlave2::printTitle_P(const prog_char* const label)
{
  uint8_t i = 0;
  _lcd.setCursor(LCDPOSITION_TITLE_XY);
  while (i<LCDPOSITION_TITLE_LENGTH && pgm_read_byte_near(&label[i])) {
    _lcd.print((char)pgm_read_byte_near(&label[i]));
    i++;
  }
  while (i++<LCDPOSITION_TITLE_LENGTH + LCDPOSITION_PAGE_OFFSET) {
    _lcd.moveCursorRight();
  }
  if (_menuItem<9) {
    _lcd.moveCursorRight();
  }
  LOG("page #"); LOG(_menuItem+1); LOG('/'); LOGLN(menu_len);
  _lcd.print(_menuItem+1);
  _lcd.print(LCDCHAR_PAGESEPARATOR);
  _lcd.print(menu_len);
}


void LSlave2::printInfo()
{
  LOG("item I"); LOGLN(_menuItem);
  printTitle_P(PSTR("IP"));
  _lcd.setCursor(0, 1);
  //#if USE_ETH
  _lcd.print(Ethernet.localIP());
  /*
  #else
  _lcd.print("--:--:--");
  #endif USE_ETH
  */
}


void LSlave2::printMessage()
{
  uint8_t index = _menuItem-NUMBEROFMENU_HOME;
  LOG("item M"); LOGLN(index);
  //printTitle_P(STATIC_MESSAGES[index].getLabel());
  _lcd.setCursor(0, 1);
  _lcd.write("TODO"); // TODO
}


void LSlave2::printPulse()
{
  uint8_t index = _menuItem - NUMBEROFMENU_HOME - STATIC_MESSAGES_LEN;
  LOG("item P"); LOGLN(index);
  printTitle_P(STATIC_PULSES[index].getLabel());
  _lcd.setCursor(LCDPOSITION_ANALOG_X, 1);
  _lcd.write(LCDCHAR_LEFTBAR);
  uint8_t valueLeft = STATIC_PULSES[index].getValue();
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
    _lcd.moveCursorRight();
  }
  _lcd.print(STATIC_PULSES[index].getValue());
}


void LSlave2::printDigital()
{
  uint8_t index = _menuItem - NUMBEROFMENU_HOME - STATIC_MESSAGES_LEN - STATIC_PULSES_LEN;
  char on, off;
  LOG("item D"); LOGLN(index);
  printTitle_P(STATIC_DIGITALS[index].getLabel());
  _lcd.setCursor(LCDPOSITION_DIGITAL_X, 1);
  if (STATIC_DIGITALS[index].getValue()) {
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
    _lcd.moveCursorRight();
  }
  _lcd.write(off);
  _lcd.write("OFF");
  _lcd.write(off);
}


const boolean LSlave2::hasNewPulsedKey()
{
  Key currentKey = getKey();
  if (_key != currentKey) {
    _key = currentKey;
    return true;
  }
  return false;
}


const LSlave2::Key LSlave2::getKey()
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


void LSlave2::add(const int8_t delta)
{
}
