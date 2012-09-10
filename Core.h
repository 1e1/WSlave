#ifndef CORE_H_
#define CORE_H_


#include <Arduino.h>
#include "config.h"
#include "macros.h"


#define MASK_PIN(pin)           (pin & B01111111)

#define DIGITAL_BITVALUE        7
#define DIGITAL_VALUE_AT(i)     (bitRead(Core::digitals[i].vPin, DIGITAL_BITVALUE))
#define PULSE_VALUE_AT(i)       (Core::pulses[i].value)




// 3 bytes = byte + @char
// readOnly
struct intfDigital
{
  // mega has 0..69 pin
  // byte [0..255]
  // digital values: 0..1
  // first left byte is writeAccess
  // second left byte is the value
  // bitRead(value, bit)
  // bitSet(value, bit)
  // bitClear(value, bit)
  // bitWrite(value, bit, bitvalue)
  byte vPin;
  const char* const label;
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
  // first left byte is writeAccess
  // digitalPinHasPWM(p)
  const byte pin;
  uint8_t value;
  const char* const label;
};
// { 13, 0, "Light" }

// 4 bytes = byte + uint8_t + @char
// readOrWrite (but not both)
struct intfMessage
{
  // mega has 0..69 pin
  // char [-128..127]
  /*const*/ char (*observe) (const byte *pins);
  char value;
  const byte* const pins;
  const char* const label;
};
// test[i].value = test[i].observe(test[i].pins)
// { obsTemperature, "##", { 13 }, "Outdoor" }


namespace Core {

  /** ===================== **/
  /**      connections      **/
  /** ===================== **/
  static intfDigital digitals[] = {
    WSDIGITAL("relay1.1", 22),
    WSDIGITAL("relay1.2", 24),
    WSDIGITAL("relay1.3", 26),
    WSDIGITAL("relay1.4", 28),
    WSDIGITAL("relay1.5", 30),
    WSDIGITAL("relay1.6", 32),
    WSDIGITAL("relay1.7", 34),
    WSDIGITAL("relay1.8", 36)
  };
  static intfPulse pulses[] = {
    WSPULSE("Pulse", 24)
  };
  static intfMessage messages[] = {};
  
  static const uint8_t digitals_len = ARRAYLEN(digitals);
  static const uint8_t pulses_len   = ARRAYLEN(pulses);
  static const uint8_t messages_len = ARRAYLEN(messages);
  
  void setup();
  void pinToChars(uint8_t pin, char out[2]);
  void setDigitalAtPin(uint8_t pin, boolean value);
  void setPulseAtPin(uint8_t pin, uint8_t value);
  
};



#endif CORE_H_
