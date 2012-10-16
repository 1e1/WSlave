#ifndef SCHEDULE_H_
#define SCHEDULE_H_



#include <Arduino.h>
#include <avr/pgmspace.h>
#include "macros.h"
#include "Connector.h"


/** 
  * unsigned int  : schedule
  * prog_char*    : label
  * uint8_t[]     : pins
  * 
  * schedule = PPPPPPPP-PPPPAAAA AAAAAAAA-DDDDDDDB
  * A : AM hour 123456789AB
  * P : PM hour 123456789AB
  * D : day MTWTFSS
  * B : boolean is DST
  * 
  */

#define BIT_DAY_OF(dayOfWeek)   (byte(1)+ (byte(dayOfWeek) % byte(7)))
#define BIT_HOUR_OF(hour)       (byte(8)+ (byte(hour) % byte(24)))

#define SMASK_FULLYEAR(bool)    (boolean(1) & boolean(bool))// DST & noDST
#define SMASK_DAY(dayOfWeek)    int(boolean(1) << BIT_DAY_OF(dayOfWeek))
#define SMASK_HOUR(hour)        int(boolean(1) << BIT_HOUR_OF(hour))

#define SMASK_WEEKEND           SMASK_DAY(SATURDAY) | SMASK_DAY(SUNDAY)
#define SMASK_EVERYDAY          SMASK_DAY(MONDAY) | SMASK_DAY(TUESDAY) | SMASK_DAY(WEDNESDAY) | SMASK_DAY(THURSDAY) | SMASK_DAY(FRIDAY) | SMASK_WEEKEND

#define SMASK_PIN_MIN           22
#define SMASK_PIN_MAX           49
#define SMASK_PIN(number_22_49) long(B1 << (number_22_49  - SMASK_PIN_MIN))


class Schedule : public ConnectorDigital {
  
  public:
  Schedule();
  Schedule(byte id, const prog_char* label, const boolean isNC, const unsigned int schedule, const unsigned long digitals_22_49);
  const boolean is(const boolean fullYear, const uint8_t dayOfWeek, const unsigned int hour);
  
  // inline
  //__attribute__((always_inline)) inline const byte getId()                    { return this->getPin();    };
  __attribute__((always_inline)) inline const boolean isActive()              { return this->getValue();  };
  __attribute__((always_inline)) inline const unsigned long getPins()         { return this->_pins;       };
  
  inline void setActive(const boolean v);
  inline const boolean hasPin(const uint8_t iPin);
  
  protected:
  unsigned int _schedule;
  unsigned long _pins;
  
};




/***********************************************************
 *                         INLINE                          *
 **********************************************************/


__attribute__((always_inline)) inline void Schedule::setActive(const boolean v)
{
  const boolean value = this->convertValue(v);
  this->_id.isActive = value;
}


__attribute__((always_inline)) inline const boolean Schedule::hasPin(const uint8_t iPin)
{
  return boolean(this->_pins >> (iPin  - SMASK_PIN_MIN)) & boolean(1);
}




#endif SCHEDULE_H_
