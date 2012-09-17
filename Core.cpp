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
  
  
  void processLine()
  {
    uint8_t pin, value, watchdog = digitals_len + pulses_len;
    // [0-9]+ OTHER [0-9]+ (OTHER [0-9]+ OTHER [0-9]+)
    //  LOG("GET pin #13"); LOG(" -> "); LOG(PULSE_VALUE_AT(0)); LOGLN(';');
    while (_currentStream->available() && watchdog--) {
      _readUint8(pin);
      _readUint8(value);
      setDigitalValueAtPin(pin, value) || setPulseValueAtPin(pin, value);
    }
    //  LOG("SET pin #13"); LOG(" <- "); LOG(PULSE_VALUE_AT(0)); LOGLN(';');
  }
  
  
  void setStream(Stream *inputStream)
  {
    _currentStream = inputStream;
  }
  
  
  void readUntil(char terminator)
  {
    char c;
    _unbuffer();
    while (_bufferSize < READBUFFERSIZE && (c=_currentStream->read())!=terminator && c!=-1) {
      _buffer[_bufferSize++] = c;
    }
  }
  
  
  boolean setDigitalValueAtPin(uint8_t pin, boolean value)
  {
    for (uint8_t i=0; i<digitals_len; i++) {
      if (DIGITAL_PIN_AT(i) == pin) {
        // value = value > 0; // if value >1 occurs issue
        bitWrite(digitals[i].vPin, DIGITAL_BITVALUE, value);
        digitalWrite(pin, value);
        return true;
      }
    }
    return false;
  }
  
  boolean getDigitalValueAtIndex(uint8_t index)
  {
    return DIGITAL_VALUE_AT(index);
  }
  
  
  boolean setPulseValueAtPin(uint8_t pin, uint8_t value)
  {
    for (uint8_t i=0; i<pulses_len; i++) {
      if (PULSE_PIN_AT(i) == pin) {
        pulses[i].value = value;
        analogWrite(pin, value);
        return true;
      }
    }
    return false;
  }
  
  uint8_t getPulseValueAtIndex(uint8_t index)
  {
    return PULSE_VALUE_AT(index);
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
    while (pgm_read_byte_near(&data[i]) && i<MAXLINESIZE) {
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
    } else {
      _copyToBuffer('"');
    }
  }
  
  
  void _copyJsonToBuffer_P(const char *label, const prog_char* const value, boolean hasMoreElement)
  {
    _copyToBuffer('"');
    _copyToBuffer(label);
    _copyToBuffer("\":\"");
    _copyToBuffer_P(value);
    if (hasMoreElement) {
      _copyToBuffer("\",");
    } else {
      _copyToBuffer('"');
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
  
  
  const uint8_t _bufferEqualsLength_P(const prog_char* const str)
  {
    uint8_t i = 0;
    while (i<_bufferSize && ((char)pgm_read_byte_near(&str[i]))==_buffer[i]) {
      i++;
    }
    return i;
  }
  
}
