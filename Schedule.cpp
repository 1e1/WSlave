#include "Schedule.h"

#include <Arduino.h>



Schedule::Schedule(byte id, const prog_char* label, unsigned int data, const uint8_t digitals[])
{
  this->init(id, label);
  this->_schedule = data;
  this->_digitals = digitals;
}


const boolean Schedule::is(const boolean fullYear, const uint8_t dayOfWeek, const unsigned int hour)
{
  return this->is(SMASK_FULLYEAR(fullYear) | SMASK_DAY(dayOfWeek) | SMASK_HOUR(hour));
}


void Schedule::check(const boolean fullYear, const uint8_t dayOfWeek, const unsigned int hour)
{
  this->setActive(this->is(fullYear, dayOfWeek, hour));
}
