#ifndef CORE2_H_
#define CORE2_H_



#include <Arduino.h>
#include <avr/pgmspace.h>
#include "config.h"
#include "macros.h"
#include "dictionary.h"
#include "Connector.h"
#include "Schedule.h"


#define MONDAY    0 // MONTAG
#define TUESDAY   1 // DIENSTAG
#define WEDNESDAY 2 // MITTWOCH
#define THURSDAY  3 // DONNERSTAG
#define FRIDAY    4 // FREITAG
#define SATURDAY  5 // SAMSTAG
#define SUNDAY    6 // SONNTAG

#define READBUFFERSIZE          max(16, 1+ ARRAYLEN(HTTP_AUTH64))
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
  static void copyToBuffer  (uint8_t x);
  static void copyToBuffer  (char c);
  static void copyToBuffer_P(const prog_char* const data);
  static void copyToBuffer  (const char chars[], uint8_t size);
  static void copyToBuffer_P(const prog_uchar data[], size_t size);
  static const uint8_t bufferEqualsLength_P(const prog_char* str);
  static void sendBuffer();
  
  static ConnectorDigital digitals[];
  static ConnectorPulse   pulses[];
  static Schedule         schedules[];

  static const uint8_t digitals_len;
  static const uint8_t pulses_len;
  static const uint8_t messages_len;
  static const uint8_t schedules_len;
  static const uint8_t total_len;
  
  static inline void printBuffer() {for(uint8_t i=0; i<_bufferSize; i++) Serial.print(_buffer[i]); };
  
  // inline
  static void setStream(Stream* inputStream);
  static void unbuffer();
  static const boolean  bufferIsEqualTo_P(const prog_char* str);
  static const uint8_t bufferIsPrefixOf_P(const prog_char* str);
  
  protected:
  static void autoSendBuffer();
  static uint8_t readUint8();
  static uint8_t getConnectorIndexOfPin(uint8_t pin, Connector connectors[], const uint8_t size);
  
  static Stream*  _currentStream;
  static char     _buffer[max(READBUFFERSIZE, WRITEBUFFERSIZE)];
  static uint8_t _bufferSize;
  
};




/***********************************************************
 *                         INLINE                          *
 **********************************************************/


__attribute__((always_inline)) inline void Core2::setStream(Stream* inputStream)
{
  Core2::_currentStream = inputStream;
}


__attribute__((always_inline)) inline void Core2::unbuffer()
{
  Core2::_bufferSize = 0;
}


__attribute__((always_inline)) inline const boolean Core2::bufferIsEqualTo_P(const prog_char* str)
{
  return Core2::_bufferSize == strlen_P(str) && strlen_P(str) == Core2::bufferEqualsLength_P(str);
};


__attribute__((always_inline)) inline const uint8_t Core2::bufferIsPrefixOf_P(const prog_char* str)
{
  return Core2::bufferEqualsLength_P(str) == strlen_P(str);
};




#endif CORE2_H_
