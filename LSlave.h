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


#define KEYPAD_VALUES 30, 150, 360, 535, 760



namespace LSlave {
  
  enum Key { KEYPAD_NONE, KEYPAD_RIGHT, KEYPAD_UP, KEYPAD_DOWN, KEYPAD_LEFT, KEYPAD_SELECT };
  
  void begin();
  void check();
  void uncheck();
//  void shutdown();
  const Key getKey();
  
  LONGBYTES(lcd_keys) = { KEYPAD_VALUES };
  static LiquidCrystal _lcd(LCD_PINS);
  static char _lcdLines[LCD_HEIGHT][LCD_WIDTH];
  
};


#endif
