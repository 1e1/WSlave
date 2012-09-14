/** 
  * 
  *   01234567890ABCDEF
  *  +-----------------+
  * 0|<NAME__9>  X##/PP|0
  * 1|<VALUE________16>|1
  *  +-----------------+
  *   01234567890ABCDEF
  * 
  * UP/DOWN: page select
  * LEFT/RIGHT: change value
  * SELECT: switch between INFO, MESSAGES, PULSES, DIGITALS
  * 
  * DIGITAL VALUE:
  * 1|    ON    off    |
  *  +-----------------+
  * ANALOG VALUE:
  * 1| -<8 bar >+  ### |
  *  +-----------------+
  * MESSAGE VALUE:
  * 1| [<8 bar >]  ### |
  *  +-----------------+
  * 
  */

#ifndef LSLAVE_H_
#define LSLAVE_H_

#include <Arduino.h>
#include <LiquidCrystal.h>
#include "Core.h"
#include "config.h"
#include "macros.h"


#define KEYPAD_MAXVALUE_RIGHT   30
#define KEYPAD_MAXVALUE_UP      150
#define KEYPAD_MAXVALUE_DOWN    360
#define KEYPAD_MAXVALUE_LEFT    535
#define KEYPAD_MAXVALUE_SELECT  760

// 255 / 17 fragments <=> 1 step = 15
// analog value: min=0, max=255, step=15
#define ANALOGSTEP              15

#define LCDCHAR_VOIDBAR         ((uint8_t) 0)
#define LCDCHAR_HALFBAR         ((uint8_t) 1)
#define LCDCHAR_FULLBAR         ((uint8_t) 2)
#define LCDCHAR_LEFTBAR         ((uint8_t) 3)
#define LCDCHAR_RIGHTBAR        ((uint8_t) 4)
#define LCDCHAR_NOLEFTBAR       ((uint8_t) 5)
#define LCDCHAR_NORIGHTBAR      ((uint8_t) 6)
#define LCDCHAR_LEFTBAR_CONTENT    { \
  B00001, \
  B00011, \
  B00111, \
  B01111, \
  B00111, \
  B00011, \
  B00001, }
#define LCDCHAR_NOLEFTBAR_CONTENT    { \
  B00001, \
  B00001, \
  B00001, \
  B00001, \
  B00001, \
  B00001, \
  B00001, }
#define LCDCHAR_VOIDBAR_CONTENT    { \
  B11111, \
  B00000, \
  B00000, \
  B00000, \
  B00000, \
  B00000, \
  B11111, }
#define LCDCHAR_HALFBAR_CONTENT    { \
  B11111, \
  B11000, \
  B11000, \
  B11000, \
  B11000, \
  B11000, \
  B11111, }
#define LCDCHAR_FULLBAR_CONTENT    { \
  B11111, \
  B11011, \
  B11011, \
  B11011, \
  B11011, \
  B11011, \
  B11111, }
#define LCDCHAR_RIGHTBAR_CONTENT    { \
  B10000, \
  B01000, \
  B00100, \
  B00010, \
  B00100, \
  B01000, \
  B10000, }
#define LCDCHAR_NORIGHTBAR_CONTENT    { \
  B10000, \
  B10000, \
  B10000, \
  B10000, \
  B10000, \
  B10000, \
  B10000, }

#define NUMBEROFMENU_HOME       1



namespace LSlave {
  
  typedef enum { KEYPAD_NONE, KEYPAD_RIGHT, KEYPAD_UP, KEYPAD_DOWN, KEYPAD_LEFT, KEYPAD_SELECT } Key;
  static const uint8_t menu_len = NUMBEROFMENU_HOME + Core::digitals_len + Core::pulses_len + Core::messages_len;
  
  void begin();
  void check();
  void uncheck();
  void shutdown();
  
  static LiquidCrystal _lcd(LCD_PINS, LCD_BLPIN);
  static char _lcdLines[LCD_HEIGHT][LCD_WIDTH];
  
  const boolean _hasNewPulsedKey();
  const Key _getKey();
  void _set(const uint8_t menuItem, const uint8_t value);
  
  static Key _key = KEYPAD_NONE;
  static uint8_t _menuItem = 0;
  
};


#endif
