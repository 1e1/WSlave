#ifndef CONNECTOR_H_
#define CONNECTOR_H_



#include <Arduino.h>
#include <avr/pgmspace.h>


class Connector {
  
  public:
  Connector();
  Connector(byte pin, const prog_char *label);
  
  __attribute__((always_inline)) inline virtual const byte getPin()         { return (const byte) _pin;    };
  __attribute__((always_inline)) inline virtual const prog_char* getLabel() { return _label;  };
  
  protected:
  void init(byte pin, const prog_char *label);
  
  byte _pin;
  const prog_char *_label PROGMEM;
  
};



#endif CONNECTOR_H_
