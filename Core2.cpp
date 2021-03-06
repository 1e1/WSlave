#include "Core2.h"




/***********************************************************
 *                       PROPERTIES                        *
 **********************************************************/




ConnectorDigital Core2::digitals[] =
{ NEWDIGITAL_NC(22, relay11)
, NEWDIGITAL_NO(24, relay12)
, NEWDIGITAL_NO(26, relay13)
, NEWDIGITAL_NO(28, relay14)
, NEWDIGITAL_NC(30, relay15)
, NEWDIGITAL_NO(32, relay16)
, NEWDIGITAL_NO(34, relay17)
, NEWDIGITAL_NO(36, relay18)
/*
, NEWDIGITAL_NC(63, sendmail)
, NEWDIGITAL_NO(62, automatic)
*/
};


ConnectorPulse Core2::pulses[] =
{ NEWPULSE(13, led)
};


Schedule Core2::schedules[] =
{ NEWSCHEDULE_NC
  ( 10
  , heating1
  , SMASK_FULLYEAR(false)
  | SMASK_DAY(WEDNESDAY) | SMASK_WEEKEND
  | SMASK_HOUR(18) | SMASK_HOUR(19) | SMASK_HOUR(7)
  , SMASK_PIN(22)  | SMASK_PIN(23)  | SMASK_PIN(30)  | SMASK_PIN(31)
  )
, NEWSCHEDULE_NC
  ( 11
  , heating2
  , SMASK_FULLYEAR(false)
  | SMASK_EVERYDAY
  | SMASK_HOUR(20) | SMASK_HOUR(22) | SMASK_HOUR(24) | SMASK_HOUR(3) | SMASK_HOUR(5) | SMASK_HOUR(6)
  , SMASK_PIN(24)  | SMASK_PIN(25)  | SMASK_PIN(32)  | SMASK_PIN(33) | SMASK_PIN(34)
  )
, NEWSCHEDULE_NC
  ( 20
  , bathroom1
  , SMASK_FULLYEAR(false)
  | SMASK_EVERYDAY
  | SMASK_HOUR(17) | SMASK_HOUR(18) | SMASK_HOUR(20) | SMASK_HOUR(7) | SMASK_HOUR(8)
  , SMASK_PIN(26)  | SMASK_PIN(27)
  )
, NEWSCHEDULE_NC
  ( 21
  , bathroom2
  , SMASK_FULLYEAR(true)
  | SMASK_EVERYDAY
  | SMASK_HOUR(17) | SMASK_HOUR(18) | SMASK_HOUR(6) | SMASK_HOUR(7)
  , SMASK_PIN(28)  | SMASK_PIN(29)
  )
};


const uint8_t Core2::digitals_len   = ARRAYLEN(digitals);
const uint8_t Core2::pulses_len     = ARRAYLEN(pulses);
const uint8_t Core2::messages_len   = 0;//ARRAYLEN(messages);
const uint8_t Core2::schedules_len  = ARRAYLEN(schedules);
const uint8_t Core2::total_len      = ARRAYLEN(digitals) + ARRAYLEN(pulses)/* + ARRAYLEN(messages) */+ ARRAYLEN(schedules);

Stream* Core2::_currentStream;
char    Core2::_buffer[max(READBUFFERSIZE, WRITEBUFFERSIZE)];
uint8_t Core2::_bufferSize;





/***********************************************************
 *                         PUBLIC                          *
 **********************************************************/




void Core2::processTimer()
{
  boolean state;
  uint8_t iDigital;
  Schedule current;
  for (uint8_t index=0; index<Core2::schedules_len; index++) {
    current = Core2::schedules[index];
    
    LOG("schedule#"); LOGLN((uint8_t)current.getPin());
    LOG("  |  value: "); LOGLN((uint8_t)current.getValue());
    
    if (current.isActive()) {
      state = current.is(FastTimer2::getDst(), FastTimer2::getDayOfWeek(), FastTimer2::getHour());
      
      LOG("  |  state: "); LOGLN((boolean)state);
      LOG("  |  pins:");
      
      for (uint8_t iPin=SMASK_PIN_MIN; iPin<=SMASK_PIN_MAX; iPin++) {
        if (current.hasPin(iPin)) {
          
          LOG(" "); LOG((uint8_t)iPin);
          
          if ((iDigital=Core2::getDigitalConnectorIndexOfPin(iPin))!=uint8_t(-1)) {
            Core2::digitals[iDigital].setValue(state);
          } else {
            digitalWrite(iPin, state);
          }
        }
      }
      LOGLN();
    }
    LOG("  |--"); LOGLN();
  }
}


void Core2::processLine()
{
  char type;
  uint8_t pin, value, index, watchdog = Core2::digitals_len + Core2::pulses_len;
  // [0-9]+ OTHER [0-9]+ (OTHER [0-9]+ OTHER [0-9]+)
  while (Core2::_currentStream->available() && --watchdog) {
    type  = Core2::_currentStream->read();
    pin   = Core2::readUint8();
    value = Core2::readUint8();
    LOG("SET #"); LOG(type); LOG(pin); LOG(" <- "); LOGLN(value);
    switch (type) {
    
      case 'S':
      if ((index=Core2::getScheduleConnectorIndexOfPin(pin))!=uint8_t(-1)) {
        Core2::schedules[index].setValue(value);
      }
      break;
      
      case 'P':
      if ((index=Core2::getPulseConnectorIndexOfPin(pin))!=uint8_t(-1)) {
        Core2::pulses[index].setValue(value);
      }
      break;
      
      case 'D':
      // first, check if this digital is not controlled by a timer
      for (index=0; index<Core2::schedules_len && !Core2::schedules[index].hasPin(pin); index++);
      if (index==Core2::schedules_len && (index=Core2::getDigitalConnectorIndexOfPin(pin))!=uint8_t(-1)) {
        Core2::digitals[index].setValue(value);
      }
      break;
      
    }
  }
}


void Core2::readUntil(char terminator)
{
  char c;
  Core2::unbuffer();
  while (Core2::_bufferSize < READBUFFERSIZE && (c=Core2::_currentStream->read())!=terminator && c!=-1) {
    Core2::_buffer[Core2::_bufferSize++] = c;
  }
}


/** 
  * value from 0 to 999
  */
void Core2::copyToBuffer(uint8_t x)
{
  if (x>=10) {
    if (x>=100) {
      Core2::copyToBuffer(char('0'+(x/100)));
      x%=100;
    }
    Core2::copyToBuffer(char('0'+(x/10)));
    x%=10;
  }
  Core2::copyToBuffer(char('0'+x));
}


void Core2::copyToBuffer(const char c)
{
  Core2::_buffer[Core2::_bufferSize++] = c;
  Core2::autoSendBuffer();
}


void Core2::copyToBuffer_P(const prog_char* const data)
{
  uint8_t i = 0;
  while (pgm_read_byte_near(&data[i]) && i<MAXLINESIZE) {
    Core2::_buffer[Core2::_bufferSize++] = pgm_read_byte_near(&data[i]);
    Core2::autoSendBuffer();
    i++;
  }
}


void Core2::copyToBuffer(const char chars[], uint8_t size)
{
  do {
    Core2::_buffer[Core2::_bufferSize++] = *chars++;
    Core2::autoSendBuffer();
  } while(--size);
}


void Core2::copyToBuffer_P(const prog_uchar data[], size_t size)
{
  for (size_t i=0; i<size; i++) {
    Core2::_buffer[Core2::_bufferSize++] = pgm_read_byte_near(&data[i]);
    Core2::autoSendBuffer();
  }
}


const uint8_t Core2::bufferEqualsLength_P(const prog_char* str)
{
  uint8_t i = 0;
  while (i<Core2::_bufferSize && ((char)pgm_read_byte_near(&str[i]))==Core2::_buffer[i++]);
  return i;
}


void Core2::sendBuffer()
{
  if (Core2::_bufferSize) {
    Core2::_currentStream->write((uint8_t *)Core2::_buffer, Core2::_bufferSize);
  }
  Core2::unbuffer();
}


void Core2::sendBufferLn()
{
  Core2::copyToBuffer_P( PSTR(CRLF) );
  Core2::sendBuffer();
}




/***********************************************************
 *                        PROTECTED                        *
 **********************************************************/




void Core2::autoSendBuffer()
{
  if (Core2::_bufferSize == WRITEBUFFERSIZE) {
    Core2::sendBuffer();
  }
}


uint8_t Core2::readUint8()
{
  char c;
  uint8_t watchdog = MAXLINESIZE, out = 0;
  _read:
  if (Core2::_currentStream->available()) {
    while ((c=Core2::_currentStream->read()) && '0'<=c && c<='9' && --watchdog) {
      out = (out *10) + byte(c -'0');
    }
    if (c==-1) {
      delay(READCHAR_TIMEOUT);
      goto _read;
    }
  }
  return out;
  // return (uint8_t) _currentStream->parseInt();
}


uint8_t Core2::getConnectorIndexOfPin(uint8_t pin, Connector connectors[], const uint8_t size)
{
  for (uint8_t index=0; index<size; index++) {
    if (connectors[index].getPin() == pin) {
      return index;
    }
  }
  return uint8_t(-1);
}


uint8_t Core2::getPulseConnectorIndexOfPin(uint8_t pin)
{
  for (uint8_t index=0; index<Core2::pulses_len; index++) {
    if (Core2::pulses[index].getPin() == pin) {
      return index;
    }
  }
  return uint8_t(-1);
}


uint8_t Core2::getDigitalConnectorIndexOfPin(uint8_t pin)
{
  for (uint8_t index=0; index<Core2::digitals_len; index++) {
    if (Core2::digitals[index].getPin() == pin) {
      return index;
    }
  }
  return uint8_t(-1);
}


uint8_t Core2::getScheduleConnectorIndexOfPin(uint8_t pin)
{
  for (uint8_t index=0; index<Core2::schedules_len; index++) {
    if (Core2::schedules[index].getPin() == pin) {
      return index;
    }
  }
  return uint8_t(-1);
}
