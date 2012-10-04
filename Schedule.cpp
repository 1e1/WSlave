#include "Schedule.h"

#include <Arduino.h>


/*
Schedule::Schedule(byte id, const prog_char* label, const boolean isNC, const unsigned int data, const uint8_t* digitals)
{
  this->_pin      = (id << 2) | isNC;
  this->_label    = label;
  this->_schedule = data;
  this->_digitals = digitals;
}
*/

Schedule::Schedule(byte id, const prog_char* label, const boolean isNC, const unsigned int data, const unsigned long digitals_32_49)
{
  this->init(id, label);
  this->_id.isNC  = isNC;
  this->_schedule = data;
  this->_pins     = digitals_32_49;
}

Schedule::Schedule(byte id, const prog_char* label, const boolean isNC, const schedule data, const unsigned long digitals_32_49)
{
  this->init(id, label);
  this->_id.isNC  = isNC;
  this->_schedule2= data;
  this->_pins     = digitals_32_49;
}


const boolean Schedule::is(const boolean fullYear, const uint8_t dayOfWeek, const unsigned int hour)
{
  return this->is(SMASK_FULLYEAR(fullYear) | SMASK_DAY(dayOfWeek) | SMASK_HOUR(hour));
}
