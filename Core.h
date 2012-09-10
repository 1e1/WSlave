#ifndef CORE_H_
#define CORE_H_


#include <Arduino.h>
#include "config.h"
#include "macros.h"


#define MASK_PIN(pin)           (pin & B01111111)

#define DIGITAL_BITVALUE        7
#define DIGITAL_VALUE_AT(i)     (bitRead(Core::digitals[i].vPin, DIGITAL_BITVALUE))
#define PULSE_VALUE_AT(i)       (Core::pulses[i].value)

#define READBUFFERSIZE          8
#define WRITEBUFFERSIZE         64




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
  void readLine(Stream *inputStream);
  void pinToChars(uint8_t pin, char out[2]);
  boolean setDigitalAtPin(uint8_t pin, boolean value);
  boolean setPulseAtPin(uint8_t pin, uint8_t value);
  
  void _copyToBuffer(uint8_t x);
  /*inline */void _copyToBuffer(char c);
  void _copyToBuffer(const char* str);
  void _copyToBuffer(const char chars[], uint8_t size);
  void _copyJsonToBuffer(const char type, const char *pinChars, const char *label);
  /*inline */void _autoSendBuffer();
  /*inline */void _sendBuffer();
  void _readUint8(uint8_t &out);
  const uint8_t _bufferEqualsLength(const char *str);
  /*__attribute__((always_inline)) inline */const boolean _bufferIsEqualTo(const char *str);
  /*__attribute__((always_inline)) inline */const uint8_t _bufferIsPrefixOf(const char *str);
  /*__attribute__((always_inline)) inline */void _unbuffer();
    
  static Stream *_currentStream;
  static char _buffer[max(READBUFFERSIZE, WRITEBUFFERSIZE)];
  static uint8_t _bufferSize;
  
  
};



#endif CORE_H_
