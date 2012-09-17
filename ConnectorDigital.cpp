#include "ConnectorDigital.h"

#include <Arduino.h>
#include <avr/pgmspace.h>



ConnectorDigital::ConnectorDigital(byte pin, const prog_char *label, const boolean isNC)
{
  init((pin | (isNC<<6)), label);
}


ConnectorDigital::ConnectorDigital(byte pin, const prog_char *label, const boolean isNC, boolean value)
{
  init((pin | (value<<7) | (isNC<<6)), label);
}
