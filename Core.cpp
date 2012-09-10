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
  
  
  void setDigitalAtPin(uint8_t pin, boolean value)
  {
    for (uint8_t i=0; i<digitals_len; i++) {
      if (MASK_PIN(digitals[i].vPin) == pin) {
        // value = value > 0; // if value >1 occurs issue
        bitWrite(digitals[i].vPin, DIGITAL_BITVALUE, value);
        digitalWrite(pin, value);
        return;
      }
    }
  }
  
  
  void setPulseAtPin(uint8_t pin, uint8_t value)
  {
    for (uint8_t i=0; i<pulses_len; i++) {
      if (pulses[i].pin == pin) {
        pulses[i].value = value;
        analogWrite(pin, value);
        return;
      }
    }
  }
  
}