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

const uint8_t Core2::digitals_len = ARRAYLEN(digitals);
const uint8_t Core2::pulses_len   = ARRAYLEN(pulses);
const uint8_t Core2::messages_len = 0;//ARRAYLEN(messages);
const uint8_t Core2::total_len    = ARRAYLEN(digitals) + ARRAYLEN(pulses);// + ARRAYLEN(messages);

Stream* Core2::_currentStream;
char    Core2::_buffer[max(READBUFFERSIZE, WRITEBUFFERSIZE)];
uint8_t Core2::_bufferSize;





/***********************************************************
 *                         PUBLIC                          *
 **********************************************************/




void Core2::processLine()
{
  uint8_t pin, value, index, watchdog = Core2::digitals_len + Core2::pulses_len;
  // [0-9]+ OTHER [0-9]+ (OTHER [0-9]+ OTHER [0-9]+)
  while (Core2::_currentStream->available() && watchdog--) {
    pin   = Core2::readUint8();
    value = Core2::readUint8();
    LOG("SET #"); LOG(pin); LOG(" <- "); LOGLN(value);
    if ((index=Core2::getConnectorIndexOfPin(pin, Core2::digitals, Core2::digitals_len))!=uint8_t(-1)) {
      LOG(">DGET #"); LOG(digitals[index].getPin()); LOG(" = "); LOGLN(digitals[index].getValue());
      Core2::digitals[index].setValue(value);
      LOG("<DGET #"); LOG(digitals[index].getPin()); LOG(" = "); LOGLN(digitals[index].getValue());
    } else if ((index=Core2::getConnectorIndexOfPin(pin, Core2::pulses, Core2::pulses_len))!=uint8_t(-1)) {
      LOG(">PGET #"); LOG(pulses[index].getPin()); LOG(" = "); LOGLN(pulses[index].getValue());
      Core2::pulses[index].setValue(value);
      LOG("<PGET #"); LOG(pulses[index].getPin()); LOG(" = "); LOGLN(pulses[index].getValue());
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
      out = (out *10) + ((uint8_t) (c -'0'));
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
