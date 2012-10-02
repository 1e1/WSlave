#include "Schedule.h"

#include <Arduino.h>



Schedule::Schedule(byte id, const prog_char* label, unsigned int data)
{
  this->init(id, label);
  this->_schedule = data;
}

/*
const boolean Schedule::is(const uint8_t dayOfWeek, const unsigned int hour)
{
  return this->is(SMASK_DAY(dayOfWeek) | SMASK_HOUR(hour));
}
*/

const boolean Schedule::is(const boolean fullYear, const uint8_t dayOfWeek, const unsigned int hour)
{
  return this->is(SMASK_FULLYEAR(fullYear) | SMASK_DAY(dayOfWeek) | SMASK_HOUR(hour));
}
