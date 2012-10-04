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
  __attribute__((always_inline)) inline const boolean isNormalyClose()                  { return this->_pin & B1;           };
  
  // hardwareValue to humanValue OR humanValue to hardwareValue
  __attribute__((always_inline)) inline const boolean convertValue(const boolean v)     { return v^this->isNormalyClose();  };
  
};




/***********************************************************
 *                         INLINE                          *
 **********************************************************/


__attribute__((always_inline)) inline const boolean ConnectorDigital::getValue()
{
  return this->convertValue( bitRead_boolean(this->_pin, 1) );
}


__attribute__((always_inline)) inline void ConnectorDigital::setValue(const boolean v)
{
  const boolean value = this->convertValue(v);
  bitWrite_boolean(this->_pin, 1, value);
  digitalWrite    (this->_pin, value);
}



#endif CONNECTORDIGITAL_H_
