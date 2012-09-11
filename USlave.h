/** 
  * READ: <pinNumber> '=' <value> ('&' <pinNumber> '=' <value>)*
  * 
  */

#ifndef USLAVE_H_
#define USLAVE_H_

#include <Arduino.h>
#include "Core.h"
#include "config.h"
#include "macros.h"



namespace USlave {
  
  void begin();
  void check();
  
  static void _sendStatus();
  
};


#endif
