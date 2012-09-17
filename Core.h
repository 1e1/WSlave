#ifndef CORE_H_
#define CORE_H_


#include <Arduino.h>
#include "config.h"
#include "macros.h"


#define MASK_PIN(pin)           (pin & B00111111)

#define DIGITAL_BITVALUE        7
#define DIGITAL_BITNC           6 // Normaly Close
#define DIGITAL_LABEL_AT(i)     (Core::digitals[i].label)
#define DIGITAL_PIN_AT(i)       (MASK_PIN(Core::digitals[i].vnPin))
#define DIGITAL_VALUE_AT(i)     (bitRead(Core::digitals[i].vnPin, DIGITAL_BITVALUE))
#define DIGITAL_ISNC_AT(i)      (bitRead(Core::digitals[i].vnPin, DIGITAL_BITNC))
#define PULSE_LABEL_AT(i)       (Core::pulses[i].label)
#define PULSE_PIN_AT(i)         (Core::pulses[i].pin)
#define PULSE_VALUE_AT(i)       (Core::pulses[i].value)
#define MESSAGE_LABEL_AT(i)     (Core::messages[i].label)

#define READBUFFERSIZE          8
#define WRITEBUFFERSIZE         64
#define MAXLINESIZE             255

#define READCHAR_TIMEOUT        4




// 3 bytes = byte + @char
// readOnly
struct intfDigital
{
  // mega has 0..69 pin
  // byte [0..255]
  // digital values: 0..1
  // first left byte is the value
  // second left byte is NC
  // bitRead(value, bit)
  // bitSet(value, bit)
  // bitClear(value, bit)
  // bitWrite(value, bit, bitvalue)
  byte vnPin;
  const prog_char *label PROGMEM;
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
  const prog_char *label PROGMEM;
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
  const prog_char *label PROGMEM;
};
// test[i].value = test[i].observe(test[i].pins)
// { obsTemperature, "##", { 13 }, "Outdoor" }


namespace Core {

  /** ===================== **/
  /**      connections      **/
  /** ===================== **/
  static intfDigital digitals[] = {
    WSDIGITAL("NC", 22, 1),
    WSDIGITAL("NO", 24, 0),
    WSDIGITAL("relay1.3", 26, 0),
    WSDIGITAL("relay1.4", 28, 0),
    WSDIGITAL("relay1.5", 30, 0),
    WSDIGITAL("relay1.6", 32, 0),
    WSDIGITAL("relay1.7", 34, 0),
    WSDIGITAL("relay1.8", 36, 0)
  };
  static intfPulse pulses[] = {
    WSPULSE("Pulse", 13)
  };
  static intfMessage messages[] = {};
  
  static const uint8_t digitals_len = ARRAYLEN(digitals);
  static const uint8_t pulses_len   = ARRAYLEN(pulses);
  static const uint8_t messages_len = ARRAYLEN(messages);
  static const uint8_t total_len    = ARRAYLEN(digitals) + ARRAYLEN(pulses) + ARRAYLEN(messages);
  
  void setup();
  void processLine();
  void readUntil(char terminator);
  boolean setDigitalValueAtPin(uint8_t pin, boolean value);
  boolean getDigitalValueAtIndex(uint8_t index);
  boolean setPulseValueAtPin(uint8_t pin, uint8_t value);
  uint8_t getPulseValueAtIndex(uint8_t index);
  
  void _copyToBuffer(uint8_t x);
  /*inline */void _copyToBuffer(char c);
  void _copyToBuffer(const char* const str);
  void _copyToBuffer_P(const prog_char* const data);
  void _copyToBuffer(const char chars[], uint8_t size);
  void _copyToBuffer_P(const prog_uchar data[], size_t size);
  void _copyJsonToBuffer(const char *label, const char *value, boolean hasMoreElement=false);
  void _copyJsonToBuffer_P(const char *label, const prog_char* const value, boolean hasMoreElement=false);
  /*inline */void _autoSendBuffer();
  /*inline */void _sendBuffer();
  void _readUint8(uint8_t &out);
  const uint8_t _bufferEqualsLength_P(const prog_char* const str);
    
  static Stream *_currentStream;
  static char _buffer[max(READBUFFERSIZE, WRITEBUFFERSIZE)];
  static uint8_t _bufferSize;
  
  /*__attribute__((always_inline)) inline */void setStream(Stream *inputStream)                       /*{ _currentStream = inputStream; }*/;
  __attribute__((always_inline)) inline const prog_char* getDigitalLabelAtIndex(uint8_t index)    { return DIGITAL_LABEL_AT(index); };
  /*__attribute__((always_inline)) inline boolean getDigitalValueAtIndex(uint8_t index)             { return DIGITAL_VALUE_AT(index); };*/
  __attribute__((always_inline)) inline const prog_char* getPulseLabelAtIndex(uint8_t index)      { return PULSE_LABEL_AT(index); };
  /*__attribute__((always_inline)) inline uint8_t getPulseValueAtIndex(uint8_t index)               { return PULSE_VALUE_AT(index); }*/;
  __attribute__((always_inline)) inline const boolean _bufferIsEqualTo_P(const prog_char *str)    { return _bufferSize == strlen_P(str) && strlen_P(str) == _bufferEqualsLength_P(str); };
  __attribute__((always_inline)) inline const uint8_t _bufferIsPrefixOf_P(const prog_char *str)   { return _bufferEqualsLength_P(str) == strlen_P(str); };
  __attribute__((always_inline)) inline void _unbuffer()                                          { _bufferSize = 0; };
  
};



#endif CORE_H_
