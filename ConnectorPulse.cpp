#include "ConnectorPulse.h"

#include <Arduino.h>
#include <avr/pgmspace.h>




/***********************************************************
 *                         PUBLIC                          *
 **********************************************************/




ConnectorPulse::ConnectorPulse(const byte pin, const prog_char* label)
{
  this->init(pin, label);
  this->_value = 0;
}


ConnectorPulse::ConnectorPulse(const byte pin, const prog_char* label, uint8_t value)
{
  this->init(pin, label);
  this->_value = value;
}


void ConnectorPulse::addValue(const uint8_t d)
{
  this->_value = constrain(this->_value + d, 0, (uint8_t) -1);
  analogWrite(this->_pin, this->_value);
}
