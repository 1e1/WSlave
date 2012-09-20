#include "LSlave2.h"




/***********************************************************
 *                       PROPERTIES                        *
 **********************************************************/




const uint8_t LSlave2::index_info     = 0;
const uint8_t LSlave2::index_message  = index_info      + (true && Core2::messages_len);
const uint8_t LSlave2::index_pulse    = index_message   + (true && Core2::pulses_len);
const uint8_t LSlave2::index_digital  = index_pulse     + (true && Core2::digitals_len);
const uint8_t LSlave2::menu_len       = index_digital   + 1;
LiquidCrystal LSlave2::_lcd(LCD_PINS/*, LCD_BLPIN, LCD_BLPOLARITY*/);

LSlave2::Key LSlave2::_key      = KEYPAD_NONE;
LSlave2::State LSlave2::_state  = SLEEPING;
uint8_t LSlave2::_menuType      = index_info;
uint8_t LSlave2::_menuIndex     = 0;
uint8_t LSlave2::_menuMax       = menu_len;




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
      
      uint8_t deltaValue = 0;
      
      // UP/DOWN: page select
      // LEFT/RIGHT: change value
      // SELECT: switch between INFO, MESSAGES, PULSES, DIGITALS
      switch (_key) {
        case KEYPAD_UP:
        jump(-1);
        break;
        case KEYPAD_DOWN:
        jump(+1);
        break;
        case KEYPAD_LEFT:
        deltaValue = +ANALOGSTEP;
        break;
        case KEYPAD_RIGHT:
        deltaValue = -ANALOGSTEP;
        break;
        case KEYPAD_SELECT:
        // jump section
        switchMenu();
        break;
      }
      
      //_lcd.home();
      _lcd.clear();
      if (_menuType==index_info) {
        printInfo();
        goto _switchEnd;
      }
      if (_menuType==index_message) {
        printMessage();
        goto _switchEnd;
      }
      if (_menuType==index_pulse) {
        if (deltaValue) {
          Core2::pulses[_menuIndex].addValue(deltaValue);
        }
        printPulse();
        goto _switchEnd;
      }
      if (_menuType==index_digital) {
        if (deltaValue) {
          Core2::digitals[_menuIndex].setValue(deltaValue>0);
        }
        printDigital();
        goto _switchEnd;
      }
      _switchEnd:
      ;
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
      default:
      //case SLEEPING:
      //_lcd.off();
      //_menuType = 0;
      _menuType      = index_info;
      _menuIndex     = 0;
      _menuMax       = NUMBEROFMENU_HOME;
      //break;
    }
  }
}


void LSlave2::printInfo()
{
  LOG("item I"); LOGLN(_menuType);
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
  if (_menuType<9) {
    _lcd.moveCursorRight();
  }
  LOG("page #"); LOG(_menuType+1); LOG('/'); LOGLN(_menuMax);
  _lcd.print(_menuType+1);
  _lcd.print(LCDCHAR_PAGESEPARATOR);
  _lcd.print(_menuMax);
}


void LSlave2::printMessage()
{
  LOG("item M"); LOGLN(_menuIndex);
  //printTitle_P(Core2::messages[_menuIndex].getLabel());
  _lcd.setCursor(0, 1);
  _lcd.write("TODO"); // TODO
}


void LSlave2::printPulse()
{
  LOG("item P"); LOGLN(_menuIndex);
  printTitle_P(Core2::pulses[_menuIndex].getLabel());
  _lcd.setCursor(LCDPOSITION_ANALOG_X, 1);
  _lcd.write(LCDCHAR_LEFTBAR);
  uint8_t valueLeft = Core2::pulses[_menuIndex].getValue();
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
  _lcd.print(Core2::pulses[_menuIndex].getValue());
}


void LSlave2::printDigital()
{
  char on, off;
  LOG("item D"); LOGLN(_menuIndex);
  printTitle_P(Core2::digitals[_menuIndex].getLabel());
  _lcd.setCursor(LCDPOSITION_DIGITAL_X, 1);
  if (Core2::digitals[_menuIndex].getValue()) {
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


void LSlave2::switchMenu()
{
  _menuIndex = 0;
  _menuType = (_menuType+1) % menu_len;
  if (_menuType==index_info) {
    _menuMax = NUMBEROFMENU_HOME;
    goto _switchEnd;
  }
  if (_menuType==index_message) {
    _menuMax = Core2::messages_len;
    goto _switchEnd;
  }
  if (_menuType==index_pulse) {
    _menuMax = Core2::pulses_len;
    goto _switchEnd;
  }
  if (_menuType==index_digital) {
    _menuMax = Core2::digitals_len;
    goto _switchEnd;
  }
  _switchEnd:
  ;
}
