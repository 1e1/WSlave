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
  * B : boolean isActive
  * 
  */

#define BIT_DAY_OF(dayOfWeek) (1+ (dayOfWeek%7))
#define BIT_HOUR_OF(hour)     (8+ (hour%24))

// cache for time test
#define MASK_ISACTIVE   B1

#define MASK_DAY(dayOfWeek)   int(B1 << BIT_DAY_OF(dayOfWeek))
#define MASK_HOUR(hour)       int(B1 << BIT_HOUR_OF(hour))


class Schedule : public ConnectorDigital {
  
  public:
  Schedule(byte id, const prog_char* label, unsigned int data);
  
  // inline
  const boolean     isActive();
  const boolean     is(const unsigned int mask);
  void setActive(const boolean value);
  
  __attribute__((always_inline)) inline const byte getId() { return this->getPin(); };
  
  protected:
  unsigned int _schedule;
  const prog_char* _label PROGMEM;
  
};




/***********************************************************
 *                         INLINE                          *
 **********************************************************/


__attribute__((always_inline)) inline const boolean Schedule::isActive()
{
  return this->is(MASK_ISACTIVE);
}


__attribute__((always_inline)) inline const boolean Schedule::is(const unsigned int mask)
{
  return this->_schedule & mask;
}


__attribute__((always_inline)) inline void Schedule::setActive(const boolean value)
{
  bitWrite_boolean(this->_schedule, 0, value);
}




#endif SCHEDULE_H_
