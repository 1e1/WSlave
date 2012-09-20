#ifndef CORE2_H_
#define CORE2_H_



#include <Arduino.h>
#include <avr/pgmspace.h>
#include "config.h"
#include "macros.h"
#include "dictionary.h"
#include "Connector.h"

#define READBUFFERSIZE          16
#define WRITEBUFFERSIZE         64
#define MAXLINESIZE             255

#define READCHAR_TIMEOUT        4


/** ===================== **/
/**      connections      **/
/** ===================== **/

/*
// DRAFT
static const byte temperaturePins[] = { 13 };
const intfMessage STATIC_MESSAGES[] = {
  { obsTemperature, 'x', temperaturePins, "Indoor" }
};
*/




class Core2 {
  
  public:
  static void processLine();
  static void readUntil(char terminator);
  static void copyToBuffer(uint8_t x);
  static /*inline */void copyToBuffer(char c);
  static void copyToBuffer_P(const prog_char* const data);
  static void copyToBuffer(const char chars[], uint8_t size);
  static void copyToBuffer_P(const prog_uchar data[], size_t size);
  static const uint8_t bufferEqualsLength_P(const prog_char* str);
  static /*inline */void sendBuffer();
  
  static ConnectorDigital digitals[];
  static ConnectorPulse pulses[];

  static const uint8_t digitals_len;
  static const uint8_t pulses_len;
  static const uint8_t messages_len;
  static const uint8_t total_len;
  
  // inline
  static void setStream(Stream *inputStream);
  static void unbuffer();
  static const boolean bufferIsEqualTo_P(const prog_char *str);
  static const uint8_t bufferIsPrefixOf_P(const prog_char *str);
  
  protected:
  static /*inline */void autoSendBuffer();
  static uint8_t readUint8();
  static uint8_t getConnectorIndexOfPin(uint8_t pin, Connector connectors[], const uint8_t size);
  
  static Stream *_currentStream;
  static char _buffer[max(READBUFFERSIZE, WRITEBUFFERSIZE)];
  static uint8_t _bufferSize;
  
};




/***********************************************************
 *                         INLINE                          *
 **********************************************************/


__attribute__((always_inline)) inline void Core2::setStream(Stream *inputStream)
{
  _currentStream = inputStream;
}


__attribute__((always_inline)) inline void Core2::unbuffer()
{
  _bufferSize = 0;
}


__attribute__((always_inline)) inline const boolean Core2::bufferIsEqualTo_P(const prog_char *str)
{
  return _bufferSize == strlen_P(str) && strlen_P(str) == bufferEqualsLength_P(str);
};


__attribute__((always_inline)) inline const uint8_t Core2::bufferIsPrefixOf_P(const prog_char *str)
{
  return bufferEqualsLength_P(str) == strlen_P(str);
};




#endif CORE2_H_
