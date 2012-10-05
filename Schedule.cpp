#include "Schedule.h"

#include <Arduino.h>


Schedule::Schedule()
{
}


Schedule::Schedule(byte id, const prog_char* label, const boolean isNC, const unsigned int schedule, const unsigned long digitals_22_49)
{
  this->init(id, label);
  this->_id.isNC  = isNC;
  this->_schedule = schedule;
  this->_pins     = digitals_22_49;
  for (uint8_t iPin=SMASK_PIN_MIN; iPin<=SMASK_PIN_MAX; iPin++) {
    if (this->hasPin(iPin)) {
      pinMode(iPin, OUTPUT);
    }
  }
}


const boolean Schedule::is(const boolean fullYear, const uint8_t dayOfWeek, const unsigned int hour)
{
  return (this->_schedule & SMASK_HOUR(hour)        )
      && (this->_schedule & SMASK_DAY(dayOfWeek)    )
      && (this->_schedule & SMASK_FULLYEAR(fullYear));
}
