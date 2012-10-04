#include "Schedule.h"

#include <Arduino.h>



Schedule::Schedule(byte id, const prog_char* label, const boolean isNC, unsigned int data, uint8_t size, uint8_t* digitals)
{
  this->_pin      = (id << 2) | isNC;
  this->_label    = label;
  this->_schedule = data;
  this->_size     = size;
  this->_digitals = digitals;
}


const boolean Schedule::is(const boolean fullYear, const uint8_t dayOfWeek, const unsigned int hour)
{
  return this->is(SMASK_FULLYEAR(fullYear) | SMASK_DAY(dayOfWeek) | SMASK_HOUR(hour));
}
