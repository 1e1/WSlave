#include "LSlave2.h"




/***********************************************************
 *                       PROPERTIES                        *
 **********************************************************/




LCDCHAR(lcdchar_voidBar)    = LCDCHAR_VOIDBAR_CONTENT;
LCDCHAR(lcdchar_halfBar)    = LCDCHAR_HALFBAR_CONTENT;
LCDCHAR(lcdchar_fullBar)    = LCDCHAR_FULLBAR_CONTENT;
LCDCHAR(lcdchar_leftBar)    = LCDCHAR_LEFTBAR_CONTENT;
LCDCHAR(lcdchar_rightBar)   = LCDCHAR_RIGHTBAR_CONTENT;
LCDCHAR(lcdchar_noLeftBar)  = LCDCHAR_NOLEFTBAR_CONTENT;
LCDCHAR(lcdchar_noRightBar) = LCDCHAR_NORIGHTBAR_CONTENT;

const uint8_t LSlave2::index_info     = 0;
const uint8_t LSlave2::index_message  = LSlave2::index_info      + (true && Core2::messages_len);
const uint8_t LSlave2::index_pulse    = LSlave2::index_message   + (true && Core2::pulses_len);
const uint8_t LSlave2::index_digital  = LSlave2::index_pulse     + (true && Core2::digitals_len);
const uint8_t LSlave2::menu_len       = LSlave2::index_digital   + 1;
LiquidCrystal LSlave2::_lcd(LCD_PINS/*, LCD_BLPIN, LCD_BLPOLARITY*/);

LSlave2::Key    LSlave2::_key   = KEYPAD_NONE;
LSlave2::State  LSlave2::_state = SLEEPING;
uint8_t LSlave2::_menuType      = LSlave2::index_info;
uint8_t LSlave2::_menuIndex     = 0;
uint8_t LSlave2::_menuMax       = LSlave2::menu_len;




/***********************************************************
 *                         PUBLIC                          *
 **********************************************************/




void LSlave2::begin()
{
  //LSlave2::_lcd.on();
  LSlave2::_lcd.begin(LCD_WIDTH, LCD_HEIGHT);
  LSlave2::_lcd.createChar_P(LCDCHAR_VOIDBAR    , lcdchar_voidBar     );
  LSlave2::_lcd.createChar_P(LCDCHAR_HALFBAR    , lcdchar_halfBar     );
  LSlave2::_lcd.createChar_P(LCDCHAR_FULLBAR    , lcdchar_fullBar     );
  LSlave2::_lcd.createChar_P(LCDCHAR_LEFTBAR    , lcdchar_leftBar     );
  LSlave2::_lcd.createChar_P(LCDCHAR_RIGHTBAR   , lcdchar_rightBar    );
  LSlave2::_lcd.createChar_P(LCDCHAR_NOLEFTBAR  , lcdchar_noLeftBar   );
  LSlave2::_lcd.createChar_P(LCDCHAR_NORIGHTBAR , lcdchar_noRightBar  );
  LSlave2::_lcd.home();
  LOGLN("display LCD");
}


void LSlave2::check()
{
  if (LSlave2::hasNewPulsedKey() && LSlave2::_key != KEYPAD_NONE) {
    LOGLN(">>> LCD");
    
    // wakeup
    if (LSlave2::_state != AWAKE) {
      LSlave2::_state = AWAKE;
      //_lcd.on();
    } else {
      
      uint8_t deltaValue = 0;
      
      // UP/DOWN: page select
      // LEFT/RIGHT: change value
      // SELECT: switch between INFO, MESSAGES, PULSES, DIGITALS
      switch (LSlave2::_key) {
        case KEYPAD_UP:
        LSlave2::jump(-1);
        break;
        case KEYPAD_DOWN:
        LSlave2::jump(+1);
        break;
        case KEYPAD_LEFT:
        deltaValue = +ANALOGSTEP;
        break;
        case KEYPAD_RIGHT:
        deltaValue = -ANALOGSTEP;
        break;
        case KEYPAD_SELECT:
        // jump section
        LSlave2::switchMenu();
        break;
      }
      
      //LSlave2::_lcd.home();
      LSlave2::_lcd.clear();
      if (LSlave2::_menuType==LSlave2::index_info) {
        LSlave2::printInfo();
        goto _switchEnd;
      }
      if (LSlave2::_menuType==LSlave2::index_message) {
        LSlave2::printMessage();
        goto _switchEnd;
      }
      if (LSlave2::_menuType==LSlave2::index_pulse) {
        if (deltaValue) {
          Core2::pulses[_menuIndex].addValue(deltaValue);
        }
        LSlave2::printPulse();
        goto _switchEnd;
      }
      if (LSlave2::_menuType==LSlave2::index_digital) {
        if (deltaValue) {
          Core2::digitals[_menuIndex].setValue(deltaValue>0);
        }
        LSlave2::printDigital();
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
  if (LSlave2::_state != SHUTDOWN) {
    switch (LSlave2::_state) {
      case AWAKE:
      //LSlave2::_lcd.setBacklight(LCD_BACKLIGHT_SLEEP);
      LSlave2::_state = SLEEPING;
      break;
      default:
      //case SLEEPING:
      //LSlave2::_lcd.off();
      //LSlave2::_menuType = 0;
      LSlave2::_menuType  = LSlave2::index_info;
      LSlave2::_menuIndex = 0;
      LSlave2::_menuMax   = NUMBEROFMENU_HOME;
      //break;
    }
  }
}


void LSlave2::printInfo()
{
  LOG("item I"); LOGLN(_menuType);
  LSlave2::printTitle_P(PSTR("IP"));
  LSlave2::_lcd.setCursor(0, 1);
  //#if USE_ETH
  LSlave2::_lcd.print(Ethernet.localIP());
  /*
  #else
  LSlave2::_lcd.print("--:--:--");
  #endif USE_ETH
  */
}




/***********************************************************
 *                        PROTECTED                        *
 **********************************************************/




void LSlave2::printTitle_P(const prog_char* const label)
{
  uint8_t i = 0;
  LSlave2::_lcd.setCursor(LCDPOSITION_TITLE_XY);
  while (i<LCDPOSITION_TITLE_LENGTH && pgm_read_byte_near(&label[i])) {
    LSlave2::_lcd.print((char)pgm_read_byte_near(&label[i]));
    i++;
  }
  while (i++<LCDPOSITION_TITLE_LENGTH + LCDPOSITION_PAGE_OFFSET) {
    LSlave2::_lcd.moveCursorRight();
  }
  if (LSlave2::_menuType<9) {
    LSlave2::_lcd.moveCursorRight();
  }
  LOG("page #"); LOG(_menuType+1); LOG('/'); LOGLN(_menuMax);
  LSlave2::_lcd.print(LSlave2::_menuType+1);
  LSlave2::_lcd.print(LCDCHAR_PAGESEPARATOR);
  LSlave2::_lcd.print(LSlave2::_menuMax);
}


void LSlave2::printMessage()
{
  LOG("item M"); LOGLN(_menuIndex);
  //LSlave2::printTitle_P(Core2::messages[LSlave2::_menuIndex].getLabel());
  LSlave2::_lcd.setCursor(0, 1);
  LSlave2::_lcd.write("TODO"); // TODO
}


void LSlave2::printPulse()
{
  LOG("item P"); LOGLN(_menuIndex);
  LSlave2::printTitle_P(Core2::pulses[LSlave2::_menuIndex].getLabel());
  LSlave2::_lcd.setCursor(LCDPOSITION_ANALOG_X, 1);
  LSlave2::_lcd.write(LCDCHAR_LEFTBAR);
  uint8_t valueLeft = Core2::pulses[LSlave2::_menuIndex].getValue();
  for (uint8_t i=0; i<LCDPOSITION_BAR_LENGTH; i++) {
    if (valueLeft > 2* ANALOGSTEP) {
      valueLeft-= 2* ANALOGSTEP;
      LSlave2::_lcd.write(LCDCHAR_FULLBAR);
    } else if (valueLeft < ANALOGSTEP) {
      LSlave2::_lcd.write(LCDCHAR_VOIDBAR);
    } else {
      valueLeft = 0;
      LSlave2::_lcd.write(LCDCHAR_HALFBAR);
    }
  }
  LSlave2::_lcd.write(LCDCHAR_RIGHTBAR);
  for (uint8_t i=0; i<LCDPOSITION_ANALOG_OFFSET; i++) {
    LSlave2::_lcd.moveCursorRight();
  }
  LSlave2::_lcd.print(Core2::pulses[LSlave2::_menuIndex].getValue());
}


void LSlave2::printDigital()
{
  char on, off;
  LOG("item D"); LOGLN(_menuIndex);
  LSlave2::printTitle_P(Core2::digitals[LSlave2::_menuIndex].getLabel());
  LSlave2::_lcd.setCursor(LCDPOSITION_DIGITAL_X, 1);
  if (Core2::digitals[LSlave2::_menuIndex].getValue()) {
    on = '*';
    off = ' ';
  } else {
    on = ' ';
    off = '*';
  }
  LSlave2::_lcd.write(on);
  LSlave2::_lcd.write("ON");
  LSlave2::_lcd.write(on);
  for (uint8_t i=0; i<LCDPOSITION_DIGITAL_INSET; i++) {
    LSlave2::_lcd.moveCursorRight();
  }
  LSlave2::_lcd.write(off);
  LSlave2::_lcd.write("OFF");
  LSlave2::_lcd.write(off);
}


const boolean LSlave2::hasNewPulsedKey()
{
  Key currentKey = LSlave2::getKey();
  if (LSlave2::_key != currentKey) {
    LSlave2::_key = currentKey;
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
  LSlave2::_menuIndex = 0;
  LSlave2::_menuType = (LSlave2::_menuType+1) % LSlave2::menu_len;
  if (LSlave2::_menuType==LSlave2::index_info) {
    LSlave2::_menuMax = NUMBEROFMENU_HOME;
    goto _switchEnd;
  }
  if (LSlave2::_menuType==LSlave2::index_message) {
    LSlave2::_menuMax = Core2::messages_len;
    goto _switchEnd;
  }
  if (LSlave2::_menuType==LSlave2::index_pulse) {
    LSlave2::_menuMax = Core2::pulses_len;
    goto _switchEnd;
  }
  if (LSlave2::_menuType==LSlave2::index_digital) {
    LSlave2::_menuMax = Core2::digitals_len;
    goto _switchEnd;
  }
  _switchEnd:
  ;
}
