#ifndef CORE_H_
#define CORE_H_


#include <Arduino.h>
#include "config.h"
#include "macros.h"




// 3 bytes = byte + @char
// readOnly
struct intfDigital
{
  // mega has 0..69 pin
  // byte [0..255]
  // digital values: 0..1
  // first byte is the value
  // bitRead(value, bit)
  // bitSet(value, bit)
  // bitClear(value, bit)
  // bitWrite(value, bit, bitvalue)
  byte vPin;
  const char *label;
};
// { 13+0b10000000 , 0, "Light" }
// { 13+128 , 0, "Light" }
// { 13+0x80 , 0, "Light" }

// 4 bytes = byte + uint8_t + @char
// readOrWrite (but not both)
struct intfPulse
{
  // mega has 0..69 pin
  // byte [0..255]
  // analog Read values: 0..1023 -> translate value>>8
  // analog Write values: 0..255
  // first byte is writeAccess
  // digitalPinHasPWM(p)
  byte wPin;
  uint8_t value;
  const char *label;
};
// { 13, 0, "Light" }

// 4 bytes = byte + uint8_t + @char
// readOrWrite (but not both)
struct intfMessage
{
  // mega has 0..69 pin
  // char [-128..127]
  /*const*/ char (*observe) (byte *pins);
  char value;
  const byte *pins;
  const char *label;
};
// test[i].value = test[i].observe(test[i].pins)
// { obsTemperature, "##", { 13 }, "Outdoor" }



extern const intfDigital  digitals[];
extern const intfPulse    pulses[]  ;
extern const intfMessage  messages[];
extern const size_t digitals_len();
extern const size_t pulses_len();
extern const size_t messages_len();


namespace Core {
  
  // TODO
  
};



#endif CORE_H_