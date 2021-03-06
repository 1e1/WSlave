#include "Connector.h"

#include <Arduino.h>
#include <avr/pgmspace.h>




/***********************************************************
 *                         PUBLIC                          *
 **********************************************************/




Connector::Connector()
{
}


Connector::Connector(byte pin, const prog_char* label)
{
  this->init(pin, label);
}


void Connector::init(byte pin, const prog_char* label)
{
  this->_id.pin = pin;
  this->_label  = label;
  pinMode(this->getPin(), OUTPUT);
}

