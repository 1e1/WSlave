#include "ConnectorPulse.h"

#include <Arduino.h>
#include <avr/pgmspace.h>



ConnectorPulse::ConnectorPulse(const byte pin, const prog_char *label)
{
  init(pin, label);
  _value = 0;
}


ConnectorPulse::ConnectorPulse(const byte pin, const prog_char *label, boolean value)
{
  init(pin, label);
  _value = value;
}


void ConnectorPulse::addValue(const uint8_t d)
{
  _value = constrain(_value + d, 0, (uint8_t) -1);
  analogWrite(_pin, _value);
}
