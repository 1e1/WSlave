#include "Schedule.h"

#include <Arduino.h>



Schedule::Schedule()
{
}


Schedule::Schedule(unsigned int data, const prog_char* label)
{
  this->init(data, label);
}


void Schedule::init(unsigned int data, const prog_char* label)
{
  this->_schedule = data;
  this->_label    = label;
}
