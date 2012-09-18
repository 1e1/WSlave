#ifndef CONNECTORDIGITAL_H_
#define CONNECTORDIGITAL_H_



#include <Arduino.h>
#include <avr/pgmspace.h>
#include "Connector.h"


class ConnectorDigital : public Connector {
  
  public:
  ConnectorDigital(byte pin, const prog_char *label, const boolean isNC);
  ConnectorDigital(byte pin, const prog_char *label, const boolean isNC, boolean value);
  
  __attribute__((always_inline)) inline virtual const byte getPin()             { return (const byte) (_pin & B00111111); };
  __attribute__((always_inline)) inline virtual const boolean getValue()        { return (const boolean) ((_pin & B10000000) ^ isNormalyClose()); };
  __attribute__((always_inline)) inline virtual void setValue(const boolean v)  { boolean value = v^isNormalyClose(); bitWrite(_pin, 7, value); digitalWrite(_pin, value); };
  
  protected:
  __attribute__((always_inline)) inline const boolean isNormalyClose()          { return _pin & B01000000; };
  
};



#endif CONNECTORDIGITAL_H_
