#include "ConnectorDigital.h"

#include <Arduino.h>
#include <avr/pgmspace.h>



ConnectorDigital::ConnectorDigital()
{
}


ConnectorDigital::ConnectorDigital(byte pin, const prog_char* label, const boolean isNC)
{
  this->init(pin, label);
  this->_pin|= isNC;
}


ConnectorDigital::ConnectorDigital(byte pin, const prog_char* label, const boolean isNC, const boolean value)
{
  this->init(pin, label);
  this->_pin|= isNC | (value << 1);
}
