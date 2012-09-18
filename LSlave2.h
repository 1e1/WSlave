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
  * SELECT: switch between INFO, MESSAGES, PULSES, DIGITALS
  * 
  * DIGITAL VALUE:
  * 1|   ?ON?  ?OFF?   |
  *   01234567890ABCDEF
  * ANALOG VALUE:
  * 1| -<BAR___9>+ ### |
  *   01234567890ABCDEF
  * MESSAGE VALUE:
  * 1| [<BAR___9>] ### |
  *   01234567890ABCDEF
  * 
  */

#ifndef LSLAVE2_H_
#define LSLAVE2_H_

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Ethernet.h>
#include "Core2.h"
#include "config.h"
#include "macros.h"


// 255 / 17 fragments <=> 1 step = 15
// analog value: min=0, max=255, step=15
#define ANALOGSTEP                  15

#define LCD_BACKLIGHT_SLEEP         31

#define LCDPOSITION_TITLE_XY        0, 0
#define LCDPOSITION_PAGE_OFFSET     1
#define LCDPOSITION_PAGE_LENGTH     2
// "##/PP" = 1+ LCDPOSITION_PAGE_LENGTH +1+ LCDPOSITION_PAGE_LENGTH
#define LCDPOSITION_TITLE_LENGTH    LCD_WIDTH - LCDPOSITION_PAGE_OFFSET - (2*LCDPOSITION_PAGE_LENGTH) -1
#define LCDPOSITION_PAGE_X          (LCD_WIDTH - LCDPOSITION_PAGE_LENGTH -1)

#define LCDPOSITION_DESCRIPTION_XY  1, 0
// space between <ON> and <OFF>
#define LCDPOSITION_DIGITAL_INSET   3
// "<ON>  <OFF>" = 1+ STRLEN(ON) +1 +LCDPOSITION_DIGITAL_INSET+ 1+ STRLEN(OFF) +1
#define LCDPOSITION_DIGITAL_X       ((  LCD_WIDTH - (1+ strlen("ON") +1 +LCDPOSITION_DIGITAL_INSET+ 1+ strlen("OFF") +1)  )/2)
#define LCDPOSITION_ANALOG_OFFSET   2
// max(uint8_t): 255
// custom chars for 2 half-step by one char
// = ceil( 255 / (2* ANALOGSTEP) )
#define LCDPOSITION_BAR_LENGTH      9
// uint8_t: [0..255]
// int8_t: [-99..127] =>  [0x00..0x7F], [0x9D..0xFF]
#define LCDPOSITION_VALUE_LENGTH    3
#define LCDPOSITION_ANALOG_X        ((  LCD_WIDTH - (1+LCDPOSITION_BAR_LENGTH+1 +LCDPOSITION_ANALOG_OFFSET+ LCDPOSITION_VALUE_LENGTH)  )/2)

#define LCDCHAR_PAGESEPARATOR       '/'
#define LCDCHAR_VOIDBAR             ((uint8_t) 0)
#define LCDCHAR_HALFBAR             ((uint8_t) 1)
#define LCDCHAR_FULLBAR             ((uint8_t) 2)
#define LCDCHAR_LEFTBAR             ((uint8_t) 3)
#define LCDCHAR_RIGHTBAR            ((uint8_t) 4)
#define LCDCHAR_NOLEFTBAR           ((uint8_t) 5)
#define LCDCHAR_NORIGHTBAR          ((uint8_t) 6)
#define LCDCHAR_LEFTBAR_CONTENT     { \
  B00001, \
  B00011, \
  B00111, \
  B01111, \
  B01111, \
  B00111, \
  B00011, \
  B00001 }
#define LCDCHAR_NOLEFTBAR_CONTENT   { \
  B00001, \
  B00001, \
  B00001, \
  B00001, \
  B00001, \
  B00001, \
  B00001, \
  B00001 }
#define LCDCHAR_VOIDBAR_CONTENT     { \
  B11011, \
  B00000, \
  B00000, \
  B00000, \
  B00000, \
  B00000, \
  B00000, \
  B11011 }
#define LCDCHAR_HALFBAR_CONTENT     { \
  B11011, \
  B11000, \
  B11000, \
  B11000, \
  B11000, \
  B11000, \
  B11000, \
  B11011 }
#define LCDCHAR_FULLBAR_CONTENT     { \
  B11011, \
  B11011, \
  B11011, \
  B11011, \
  B11011, \
  B11011, \
  B11011, \
  B11011 }
#define LCDCHAR_RIGHTBAR_CONTENT    { \
  B10000, \
  B01000, \
  B00100, \
  B00010, \
  B00010, \
  B00100, \
  B01000, \
  B10000 }
#define LCDCHAR_NORIGHTBAR_CONTENT  { \
  B10000, \
  B10000, \
  B10000, \
  B10000, \
  B10000, \
  B10000, \
  B10000, \
  B10000 }

#define NUMBEROFMENU_HOME       1

#define LCDMENU_INFO    0
#define LCDMENU_MESSAGE 1
#define LCDMENU_PULSE   2
#define LCDMENU_DIGITAL 3
#define LCDMENU_LEN     4



class LSlave2 {
  
  public:
  static const uint8_t menu_len;
  
  typedef enum { SHUTDOWN, SLEEPING, AWAKE } State;
  typedef enum { KEYPAD_NONE, KEYPAD_RIGHT, KEYPAD_UP, KEYPAD_DOWN, KEYPAD_LEFT, KEYPAD_SELECT } Key;
  
  static void begin();
  static void check();
  static void shutdown();
  static void printInfo();
  
  // inline
  static void uncheck();
  
  protected:
  static LiquidCrystal _lcd;
  
  static void printTitle_P(const prog_char* const label);
  static void printMessage();
  static void printPulse();
  static void printDigital();
  static const boolean hasNewPulsedKey();
  static const Key getKey();
  static void add(const int8_t delta);
  static void switchMenu();
  
  // inline
  static void jump(const uint8_t inc);
  
  static Key _key;
  static uint8_t _menuType;
  static uint8_t _menuIndex;
  static uint8_t _menuMax;
  static State _state;
  
};




/***********************************************************
 *                         INLINE                          *
 **********************************************************/


__attribute__((always_inline)) inline void LSlave2::uncheck()
{
  
}


__attribute__((always_inline)) inline void LSlave2::jump(const uint8_t inc)
{
  _menuIndex = (_menuIndex+inc) % _menuMax;
}


#endif LSLAVE2_H_
