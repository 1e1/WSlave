#ifndef CONNECTORPULSE_H_
#define CONNECTORPULSE_H_



#include <Arduino.h>
#include <avr/pgmspace.h>
#include "Connector.h"
#include "macros.h"


class ConnectorPulse : public Connector {
  
  public:
  ConnectorPulse(const byte pin, const prog_char *label);
  ConnectorPulse(const byte pin, const prog_char *label, uint8_t value);
  
  void addValue(const uint8_t d);
  
  __attribute__((always_inline)) inline uint8_t getValue()        { return this->_value;                                        };
  __attribute__((always_inline)) inline void setValue(uint8_t v)  { this->_value = v; analogWrite(this->_pin, this->_value);    };
  
  protected:
  uint8_t _value;
  
};



#endif CONNECTORPULSE_H_
