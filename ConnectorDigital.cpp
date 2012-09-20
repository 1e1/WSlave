#include "ConnectorDigital.h"

#include <Arduino.h>
#include <avr/pgmspace.h>



ConnectorDigital::ConnectorDigital(byte pin, const prog_char* label, const boolean isNC)
{
  this->init((pin | (isNC<<7)), label);
}


ConnectorDigital::ConnectorDigital(byte pin, const prog_char* label, const boolean isNC, boolean value)
{
  this->init((pin | (value<<6) | (isNC<<7)), label);
}
