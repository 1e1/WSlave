#ifndef CORE2_H_
#define CORE2_H_



#include <Arduino.h>
#include <avr/pgmspace.h>
#include "config.h"
#include "macros.h"
#include "dictionary.h"
#include "Connector.h"

#define READBUFFERSIZE          8
#define WRITEBUFFERSIZE         64
#define MAXLINESIZE             255

#define READCHAR_TIMEOUT        4

#define STATIC_DIGITALS         digitals
#define STATIC_DIGITALS_LEN     digitals_len
#define STATIC_PULSES           pulses
#define STATIC_PULSES_LEN       pulses_len
#define STATIC_MESSAGES         messages
#define STATIC_MESSAGES_LEN     messages_len
#define STATIC_TOTAL_LEN        totaldpm_len


/** ===================== **/
/**      connections      **/
/** ===================== **/

static ConnectorDigital STATIC_DIGITALS[] = {
  ConnectorDigital(22, Dictionary::relay11, true) ,
  ConnectorDigital(24, Dictionary::relay12, false),
  ConnectorDigital(26, Dictionary::relay13, false),
  ConnectorDigital(28, Dictionary::relay14, false)/*
  NEWDIGITAL(22, relay11, true) ,
  NEWDIGITAL(24, relay12, false),
  NEWDIGITAL(26, relay13, false),
  NEWDIGITAL(28, relay14, false)*/
};

static ConnectorPulse STATIC_PULSES[] = {
  ConnectorPulse(13, Dictionary::led)
//  NEWPULSE(13, led)
};

/*
// DRAFT
static const byte temperaturePins[] = { 13 };
const intfMessage STATIC_MESSAGES[] = {
  { obsTemperature, 'x', temperaturePins, "Indoor" }
};
*/

static const uint8_t STATIC_DIGITALS_LEN = ARRAYLEN(STATIC_DIGITALS);
static const uint8_t STATIC_PULSES_LEN   = ARRAYLEN(STATIC_PULSES);
static const uint8_t STATIC_MESSAGES_LEN = 0;//ARRAYLEN(STATIC_MESSAGES);
static const uint8_t STATIC_TOTAL_LEN    = ARRAYLEN(STATIC_DIGITALS) + ARRAYLEN(STATIC_PULSES);// + ARRAYLEN(STATIC_MESSAGES);




class Core2 {
  
  public:
  static void processLine();
  static void readUntil(char terminator);
  static void copyToBuffer(uint8_t x);
  static /*inline */void copyToBuffer(char c);
  static void copyToBuffer(const char* const str);
  static void copyToBuffer_P(const prog_char* const data);
  static void copyToBuffer(const char chars[], uint8_t size);
  static void copyToBuffer_P(const prog_uchar data[], size_t size);
  static const uint8_t bufferEqualsLength_P(const prog_char* const str);
  static /*inline */void sendBuffer();
  
  // inline
  static void setStream(Stream *inputStream);
  static void unbuffer();
  static const boolean bufferIsEqualTo_P(const prog_char *str);
  static const uint8_t bufferIsPrefixOf_P(const prog_char *str);
  
  protected:
  static /*inline */void autoSendBuffer();
  static void readUint8(uint8_t &out);
  static uint8_t getConnectorIndexOfPin(uint8_t pin, Connector connectors[], const uint8_t size);
  
  static Stream *_currentStream;
  static char _buffer[max(READBUFFERSIZE, WRITEBUFFERSIZE)];
  static uint8_t _bufferSize;
  
  static const ConnectorDigital *_digitals;
  static const ConnectorPulse *_pulses;
  //static const ConnectorMessage *_messages;
  
  // inline
  static const ConnectorDigital getDigitalAtIndex(uint8_t index);
  static const ConnectorPulse getPulseAtIndex(uint8_t index);
  //static const ConnectorMessage* getPulseAtIndex(uint8_t index);
  
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


__attribute__((always_inline)) inline const ConnectorDigital Core2::getDigitalAtIndex(uint8_t index)
{
  return STATIC_DIGITALS[index];
};


__attribute__((always_inline)) inline const ConnectorPulse Core2::getPulseAtIndex(uint8_t index)
{
  return STATIC_PULSES[index];
};

/*
__attribute__((always_inline) inline const ConnectorMessage* Core2::getPulseAtIndex(uint8_t index)
{
  return STATIC_MESSAGES[index];
};
*/


#endif CORE2_H_
