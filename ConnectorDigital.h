#ifndef CONNECTORDIGITAL_H_
#define CONNECTORDIGITAL_H_



#include <Arduino.h>
#include <avr/pgmspace.h>
#include "macros.h"
#include "Connector.h"


class ConnectorDigital : public Connector {
  
  public:
  ConnectorDigital(byte pin, const prog_char *label, const boolean isNC);
  ConnectorDigital(byte pin, const prog_char *label, const boolean isNC, boolean value);
  
  __attribute__((always_inline)) inline const boolean getValue()        { return (const boolean) (bitRead_boolean(_pin, 1) ^ isNormalyClose()); };
  __attribute__((always_inline)) inline void setValue(const boolean v)  { const boolean value = v^isNormalyClose(); bitWrite_boolean(_pin, 1, value); digitalWrite(_pin, value); };
  
  protected:
  __attribute__((always_inline)) inline const boolean isNormalyClose()  { return _pin & B1; };
  
};



#endif CONNECTORDIGITAL_H_
