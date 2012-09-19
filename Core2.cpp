#include "Core2.h"




/***********************************************************
 *                       PROPERTIES                        *
 **********************************************************/




Stream *Core2::_currentStream;
char Core2::_buffer[max(READBUFFERSIZE, WRITEBUFFERSIZE)];
uint8_t Core2::_bufferSize;




/***********************************************************
 *                         PUBLIC                          *
 **********************************************************/




void Core2::processLine()
{
  uint8_t pin, value, index, watchdog = STATIC_DIGITALS_LEN + STATIC_PULSES_LEN;
  // [0-9]+ OTHER [0-9]+ (OTHER [0-9]+ OTHER [0-9]+)
  while (_currentStream->available() && watchdog--) {
    readUint8(pin);
    readUint8(value);
    LOG("SET #"); LOG(pin); LOG(" <- "); LOG(value);
    if ((index=getConnectorIndexOfPin(pin, STATIC_DIGITALS, STATIC_DIGITALS_LEN))!=((uint8_t)-1)) {
        LOG(" D: "); LOGLN(index);
      STATIC_DIGITALS[index].setValue(value);
    } else if ((index=getConnectorIndexOfPin(pin, STATIC_PULSES, STATIC_PULSES_LEN))!=((uint8_t)-1)) {
        LOG(" P: "); LOGLN(index);
      STATIC_PULSES[index].setValue(value);
    }
  }
}


void Core2::readUntil(char terminator)
{
  char c;
  unbuffer();
  while (_bufferSize < READBUFFERSIZE && (c=_currentStream->read())!=terminator && c!=-1) {
    _buffer[_bufferSize++] = c;
  }
}


/** 
  * value from 0 to 999
  */
void Core2::copyToBuffer(uint8_t x)
{
  char buf[4];
  uint8_t i = 3;
  buf[3] = '\0';
  do {
    buf[--i] = '0'+ (x %10);
  } while (x && i>0 && (x/=10));
  copyToBuffer(buf+i);
}


void Core2::copyToBuffer(const char c)
{
  _buffer[_bufferSize++] = c;
  autoSendBuffer();
}


void Core2::copyToBuffer(const char* const str)
{
  uint8_t i = 0;
  while (str[i] && i<MAXLINESIZE) {
    _buffer[_bufferSize++] = str[i++];
    autoSendBuffer();
  }
}


void Core2::copyToBuffer_P(const prog_char* const data)
{
  uint8_t i = 0;
  while (pgm_read_byte_near(&data[i]) && i<MAXLINESIZE) {
    _buffer[_bufferSize++] = pgm_read_byte_near(&data[i]);
    autoSendBuffer();
    i++;
  }
}


void Core2::copyToBuffer(const char chars[], uint8_t size)
{
  for (uint8_t i=0; i<size; i++) {
    _buffer[_bufferSize++] = chars[i];
    autoSendBuffer();
  }
}


void Core2::copyToBuffer_P(const prog_uchar data[], size_t size)
{
  for (size_t i=0; i<size; i++) {
    _buffer[_bufferSize++] = pgm_read_byte_near(&data[i]);
    autoSendBuffer();
  }
}


const uint8_t Core2::bufferEqualsLength_P(const prog_char* const str)
{
  uint8_t i = 0;
  while (i<_bufferSize && ((char)pgm_read_byte_near(&str[i]))==_buffer[i]) {
    i++;
  }
  return i;
}


void Core2::sendBuffer()
{
  if (_bufferSize) {
    _currentStream->write((uint8_t *)_buffer, _bufferSize);
  }
}




/***********************************************************
 *                        PROTECTED                        *
 **********************************************************/




void Core2::autoSendBuffer()
{
  if (_bufferSize == WRITEBUFFERSIZE) {
    _currentStream->write((uint8_t *)_buffer, WRITEBUFFERSIZE);
    unbuffer();
  }
}


void Core2::readUint8(uint8_t &out)
{
  char c;
  uint8_t watchdog = MAXLINESIZE;
  out = 0;
  _read:
  if (_currentStream->available()) {
    while ((c=_currentStream->read()) && '0'<=c && c<='9' && --watchdog) {
      out = (out *10) + ((uint8_t) (c -'0'));
    }
    if (c==-1) {
      delay(READCHAR_TIMEOUT);
      goto _read;
    }
  }
  // return (uint8_t) _currentStream->parseInt();
}


uint8_t Core2::getConnectorIndexOfPin(uint8_t pin, Connector connectors[], const uint8_t size)
{
  for (uint8_t index=0; index<size; index++) {
    if (connectors[index].getPin() == pin) {
      return index;
    }
  }
  return -1;
}
