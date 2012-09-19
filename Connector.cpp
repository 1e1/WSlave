#include "Connector.h"

#include <Arduino.h>
#include <avr/pgmspace.h>



Connector::Connector()
{
}


Connector::Connector(byte pin, const prog_char *label)
{
  init(pin, label);
}


void Connector::init(byte pin, const prog_char *label)
{
  _pin = pin << 2;
  _label = label;
  pinMode(_pin, OUTPUT);
}

