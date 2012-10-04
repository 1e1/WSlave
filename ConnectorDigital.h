#ifndef CONNECTORDIGITAL_H_
#define CONNECTORDIGITAL_H_



#include <Arduino.h>
#include <avr/pgmspace.h>
#include "macros.h"
#include "Connector.h"


class ConnectorDigital : public Connector {
  
  public:
  ConnectorDigital();
  ConnectorDigital(byte pin, const prog_char* label, const boolean isNC);
  ConnectorDigital(byte pin, const prog_char* label, const boolean isNC, const boolean value);
  
  // inline
  const boolean getValue();
  void setValue(const boolean v);
  
  protected:
  // hardwareValue to humanValue OR humanValue to hardwareValue
  __attribute__((always_inline)) inline const boolean convertValue(const boolean v)     { return v ^ this->_id.isNC; };
  
};




/***********************************************************
 *                         INLINE                          *
 **********************************************************/


__attribute__((always_inline)) inline const boolean ConnectorDigital::getValue()
{
  return this->convertValue( this->_id.isActive );
}


__attribute__((always_inline)) inline void ConnectorDigital::setValue(const boolean v)
{
  this->_id.isActive = this->convertValue(v);
  digitalWrite(this->getPin(), this->_id.isActive);
}



#endif CONNECTORDIGITAL_H_
