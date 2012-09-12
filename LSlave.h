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



namespace LSlave {
  
  void begin();
  void check();
  void uncheck();
//  void shutdown();
  
  static LiquidCrystal _lcd(LCD_PINS);
  static char _lcdLines[LCD_HEIGHT][LCD_WIDTH];
  
};


#endif
