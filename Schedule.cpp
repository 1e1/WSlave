#include "Schedule.h"

#include <Arduino.h>



Schedule::Schedule(byte id, const prog_char* label, unsigned int data)
{
  this->init(id, label);
  this->_schedule = data;
}
