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
  
  
  void readLine(Stream *currentStream)
  {
    uint8_t pin, value;
    _currentStream = currentStream;
    _readUint8(pin);
    _readUint8(value);
    setDigitalAtPin(pin, value) || setPulseAtPin(pin, value);
    LOG("SET pin #"); LOG(pin); LOG(" <- "); LOG(value); LOGLN(';');
  }
  
  /** 
    * pin number MUST be in 0..99
    */
  void pinToChars(uint8_t pin, char out[2])
  {
    out[1] = '0' + (pin % 10);
    if (pin > 9) {
      out[0] = '0' + (pin / 10);
    } else {
      out[0] = '0';
    }
    // LOG(pin); LOG(": "); LOG(out[0]); LOG("; "); LOG(out[1]); LOGLN("; ");
  }
  
  
  void _readUint8(uint8_t &out)
  {
    int c;
    while ((c=_currentStream->read())/*!=-1*/ && '0'<=c && 'c'<='9') {
      out = (out *10) + ((uint8_t) (c -'0'));
    }
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
  
}