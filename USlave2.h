/** 
  * READ: <pinNumber> '=' <value> ('&' <pinNumber> '=' <value>)*
  * 
  */

#ifndef USLAVE2_H_
#define USLAVE2_H_

#include <Arduino.h>
#include "Core2.h"
#include "config.h"
#include "macros.h"



class USlave2 {
  
  public:
  __attribute__((always_inline)) static inline void begin()                 { Serial.begin(USB_SPEED); LOGLN("listen USB"); };
  static void check();
  __attribute__((always_inline)) static inline void uncheck()               { Serial.flush(); };
  
  protected:
  static void statusLine(Connector connector, uint8_t value);
  
};


#endif
