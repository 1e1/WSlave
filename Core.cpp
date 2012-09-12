#include "Core.h"



namespace Core {
  
  void setup()
  {
    for (uint8_t i=0; i<digitals_len; i++) {
      //pinMode(MASK_PIN(digitals[i].vPin), OUTPUT);
      // because value is 0 at startup
      pinMode(digitals[i].vPin, OUTPUT);
    }
    for (uint8_t i=0; i<pulses_len; i++) {
      pinMode(pulses[i].pin, OUTPUT);
    }
  }
  
  
  void setStream(Stream *inputStream)
  {
    _currentStream = inputStream;
  }
  
  
  void readLine()
  {
    uint8_t pin, value, watchdog = digitals_len + pulses_len;
    // [0-9]+ OTHER [0-9]+ (OTHER [0-9]+ OTHER [0-9]+)
    while (_currentStream->available() && watchdog--) {
      _readUint8(pin);
      _readUint8(value);
      setDigitalAtPin(pin, value) || setPulseAtPin(pin, value);
      LOG("SET pin #"); LOG(pin); LOG(" <- "); LOG(value); LOGLN(';');
    }
  }
  
  
  void readLine(const char until)
  {
    Core::_unbuffer();
    _bufferSize = _currentStream->readBytesUntil(until, _buffer, READBUFFERSIZE);
  }
  
  
  boolean setDigitalAtPin(uint8_t pin, boolean value)
  {
    for (uint8_t i=0; i<digitals_len; i++) {
      if (MASK_PIN(digitals[i].vPin) == pin) {
        // value = value > 0; // if value >1 occurs issue
        bitWrite(digitals[i].vPin, DIGITAL_BITVALUE, value);
        digitalWrite(pin, value);
        return true;
      }
    }
    return false;
  }
  
  
  boolean setPulseAtPin(uint8_t pin, uint8_t value)
  {
    for (uint8_t i=0; i<pulses_len; i++) {
      if (pulses[i].pin == pin) {
        pulses[i].value = value;
        analogWrite(pin, value);
        return true;
      }
    }
    return false;
  }
  
  
  /** 
    * value from 0 to 999
    */
  void _copyToBuffer(uint8_t x)
  {
    char buf[4];
    uint8_t i = 3;
    buf[3] = '\0';
    do {
      buf[--i] = '0'+ (x %10);
    } while (x && i>0 && (x/=10));
    _copyToBuffer(buf+i);
  }
  
  
  void _copyToBuffer(const char c)
  {
    _buffer[_bufferSize++] = c;
    _autoSendBuffer();
  }
  
  
  void _copyToBuffer(const char* const str)
  {
    uint8_t i = 0;
    while (str[i] && i<MAXLINESIZE) {
      _buffer[_bufferSize++] = str[i++];
      _autoSendBuffer();
    }
  }
  
  
  void _copyToBuffer_P(const prog_char* const data)
  {
    uint8_t i = 0;
    while (pgm_read_byte(&data[i]) && i<MAXLINESIZE) {
      _buffer[_bufferSize++] = pgm_read_byte_near(&data[i]);
      _autoSendBuffer();
      i++;
    }
  }
  
  
  void _copyToBuffer(const char chars[], uint8_t size)
  {
    for (uint8_t i=0; i<size; i++) {
      _buffer[_bufferSize++] = chars[i];
      _autoSendBuffer();
    }
  }
  
  
  void _copyToBuffer_P(const prog_uchar data[], size_t size)
  {
    for (size_t i=0; i<size; i++) {
      _buffer[_bufferSize++] = pgm_read_byte_near(&data[i]);
      _autoSendBuffer();
    }
  }
  
  
  void _copyJsonToBuffer(const char *label, const char *value, boolean hasMoreElement)
  {
    _copyToBuffer('"');
    _copyToBuffer(label);
    _copyToBuffer("\":\"");
    _copyToBuffer(value);
    if (hasMoreElement) {
        _copyToBuffer("\",");
    }
  }
  
  
  void _autoSendBuffer()
  {
    if (_bufferSize == WRITEBUFFERSIZE) {
      _currentStream->write((uint8_t *)_buffer, WRITEBUFFERSIZE);
      Core::_unbuffer();
    }
  }
  
  
  void _sendBuffer()
  {
    if (_bufferSize) {
      _currentStream->write((uint8_t *)_buffer, _bufferSize);
    }
  }
  
  
  void _readUint8(uint8_t &out)
  {
    int c;
    while ((c=_currentStream->read())/*!=-1*/ && '0'<=c && 'c'<='9') {
      out = (out *10) + ((uint8_t) (c -'0'));
    }
    // return (uint8_t) _currentStream->parseInt();
  }
  
  
  void _readUntil(char terminator)
  {
    _bufferSize = _currentStream->readBytesUntil(terminator, _buffer, READBUFFERSIZE);
  }
  
  
  const uint8_t _bufferEqualsLength_P(const prog_char* const str)
  {
    uint8_t i = 0;
    while (i<_bufferSize && ((char)pgm_read_byte_near(&str[i]))==_buffer[i]) {
      i++;
    }
    return i;
  }
  
  
  const boolean _bufferIsEqualTo_P(const prog_char *str)
  {
    return _bufferSize == strlen_P(str) && strlen_P(str) == _bufferEqualsLength_P(str);
  }
  
  
  const uint8_t _bufferIsPrefixOf_P(const prog_char *str)
  {
    return _bufferEqualsLength_P(str) == strlen_P(str);
  }
  
  
  void _unbuffer()
  {
    _bufferSize = 0;
  }
  
}
