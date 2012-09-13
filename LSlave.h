/** 
  * 
  *   01234567890ABCDEF
  *  +-----------------+
  * 0|<NAME__10>  ##/PP|0
  * 1|<VALUE________16>|1
  *  +-----------------+
  *   01234567890ABCDEF
  * 
  * UP/DOWN: page select
  * LEFT/RIGHT: change value
  * SELECT: ?
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



namespace LSlave {
  
  typedef enum { KEYPAD_NONE, KEYPAD_RIGHT, KEYPAD_UP, KEYPAD_DOWN, KEYPAD_LEFT, KEYPAD_SELECT } Key;
  
  void begin();
  void check();
  void uncheck();
  void shutdown();
  
  static LiquidCrystal _lcd(LCD_PINS);
  static char _lcdLines[LCD_HEIGHT][LCD_WIDTH];
  
  const Key _getKey();
  
  static Key _pressedKey = KEYPAD_NONE;
  
};


#endif
