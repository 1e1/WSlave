#include "Core2.h"




/***********************************************************
 *                       PROPERTIES                        *
 **********************************************************/




ConnectorDigital Core2::digitals[] = {
  NEWDIGITAL_NC(22, relay11) ,
  NEWDIGITAL_NO(24, relay12),
  NEWDIGITAL_NO(26, relay13),
  NEWDIGITAL_NO(28, relay14),
  NEWDIGITAL_NC(30, relay15),
  NEWDIGITAL_NO(32, relay16),
  NEWDIGITAL_NO(34, relay17),
  NEWDIGITAL_NO(36, relay18),
  
  NEWDIGITAL_NC(63, sendmail),
  NEWDIGITAL_NO(62, automatic)
};

ConnectorPulse Core2::pulses[] = {
  NEWPULSE(13, led)
};

static ConnectorDigital noDigitals[] = {};

Schedule Core2::schedules[] = {
  NEWSCHEDULE
  ( 10
  , heating1
  , SMASK_FULLYEAR(false)
  | SMASK_DAY(WEDNESDAY) | SMASK_WEEKEND
  | SMASK_HOUR(18) | SMASK_HOUR(19) | SMASK_HOUR(7)
  , noDigitals
  ),
  NEWSCHEDULE
  ( 11
  , heating2
  , SMASK_FULLYEAR(false)
  | SMASK_EVERYDAY
  | SMASK_HOUR(20) | SMASK_HOUR(22) | SMASK_HOUR(24) | SMASK_HOUR(3) | SMASK_HOUR(5) | SMASK_HOUR(6)
  , noDigitals
  ),
  NEWSCHEDULE
  ( 20
  , bathroom1
  , SMASK_FULLYEAR(false)
  | SMASK_EVERYDAY
  | SMASK_HOUR(17) | SMASK_HOUR(18) | SMASK_HOUR(20) | SMASK_HOUR(6) | SMASK_HOUR(7)
  , noDigitals
  ),
  NEWSCHEDULE
  ( 21
  , bathroom2
  , SMASK_FULLYEAR(true)
  | SMASK_EVERYDAY
  | SMASK_HOUR(17) | SMASK_HOUR(18) | SMASK_HOUR(6) | SMASK_HOUR(7)
  , noDigitals
  )
};

const uint8_t Core2::digitals_len = ARRAYLEN(digitals);
const uint8_t Core2::pulses_len   = ARRAYLEN(pulses);
const uint8_t Core2::messages_len = 0;//ARRAYLEN(messages);
const uint8_t Core2::schedules_len= ARRAYLEN(schedules);
const uint8_t Core2::total_len    = ARRAYLEN(digitals) + ARRAYLEN(pulses)/* + ARRAYLEN(messages) */+ ARRAYLEN(schedules);

Stream* Core2::_currentStream;
char    Core2::_buffer[max(READBUFFERSIZE, WRITEBUFFERSIZE)];
uint8_t Core2::_bufferSize;





/***********************************************************
 *                         PUBLIC                          *
 **********************************************************/




void Core2::processLine()
{
  char type;
  uint8_t pin, value, index, watchdog = Core2::digitals_len + Core2::pulses_len;
  // [0-9]+ OTHER [0-9]+ (OTHER [0-9]+ OTHER [0-9]+)
  while (Core2::_currentStream->available() && watchdog--) {
    type  = Core2::_currentStream->read();
    pin   = Core2::readUint8();
    value = Core2::readUint8();
    LOG("SET #"); LOG(type); LOG(pin); LOG(" <- "); LOGLN(value);
    switch (type) {
    
      case 'S':
      if ((index=Core2::getConnectorIndexOfPin(pin, Core2::schedules, Core2::schedules_len))!=uint8_t(-1)) {
        Core2::schedules[index].setValue(value);
      }
      break;
      
      case 'P':
      if ((index=Core2::getConnectorIndexOfPin(pin, Core2::pulses, Core2::pulses_len))!=uint8_t(-1)) {
        Core2::pulses[index].setValue(value);
      }
      break;
      
      case 'D':
      if ((index=Core2::getConnectorIndexOfPin(pin, Core2::digitals, Core2::digitals_len))!=uint8_t(-1)) {
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
  while(size--) {
    Core2::_buffer[Core2::_bufferSize++] = *chars++;
    Core2::autoSendBuffer();
  }
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
