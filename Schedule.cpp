#include "Schedule.h"

#include <Arduino.h>



Schedule::Schedule(byte id, const prog_char* label, unsigned int data)
{
  this->init(id, label);
  this->_schedule = data;
}


const boolean Schedule::is(const uint8_t dayOfWeek, const unsigned int hour)
{
  return this->is(BIT_DAY_OF(dayOfWeek) | BIT_HOUR_OF(hour));
}
