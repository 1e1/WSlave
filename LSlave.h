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

#define LIGHTON HIGH
#define LIGHTOFF LOW



namespace LSlave {
  
  typedef enum { KEYPAD_NONE, KEYPAD_RIGHT, KEYPAD_UP, KEYPAD_DOWN, KEYPAD_LEFT, KEYPAD_SELECT } Key;
  static const uint8_t menu_len = ARRAYLEN(digitals) + ARRAYLEN(pulses) + ARRAYLEN(messages) +1 /* ETH0 */;
  
  void begin();
  void check();
  void uncheck();
  void shutdown();
  
  static LiquidCrystal _lcd(LCD_PINS);
  static char _lcdLines[LCD_HEIGHT][LCD_WIDTH];
  
  const boolean _hasNewPulsedKey();
  const Key _getKey();
  
  static Key _key = KEYPAD_NONE;
  static uint8_t menuItem = 0;
  
};


#endif
