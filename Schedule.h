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

#define BIT_DAY_OF(dayOfWeek) (byte(1)+ (byte(dayOfWeek) % byte(7)))
#define BIT_HOUR_OF(hour)     (byte(8)+ (byte(hour) % byte(24)))

#define SMASK_FULLYEAR(bool)  (boolean(1) & boolean(bool))// DST & noDST
#define SMASK_DAY(dayOfWeek)  int(boolean(1) << BIT_DAY_OF(dayOfWeek))
#define SMASK_HOUR(hour)      int(boolean(1) << BIT_HOUR_OF(hour))


class Schedule : public ConnectorDigital {
  
  public:
  Schedule(byte id, const prog_char* label, unsigned int data);
  //const boolean     is(const uint8_t dayOfWeek, const unsigned int hour);
  const boolean     is(const boolean fullYear, const uint8_t dayOfWeek, const unsigned int hour);
  
  // inline
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


__attribute__((always_inline)) inline const boolean Schedule::is(const unsigned int mask)
{
  return this->_schedule & mask;
}


__attribute__((always_inline)) inline void Schedule::setActive(const boolean value)
{
  bitWrite_boolean(this->_schedule, boolean(0), value);
}




#endif SCHEDULE_H_
