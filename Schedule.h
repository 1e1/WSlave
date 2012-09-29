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

#define BIT_MONDAY      1 // MONTAG
#define BIT_TUESDAY     2 // DIENSTAG
#define BIT_WEDNESDAY   3 // MITTWOCH
#define BIT_THURSDAY    4 // DONNERSTAG
#define BIT_FRIDAY      5 // FREITAG
#define BIT_SATURDAY    6 // SAMSTAG
#define BIT_SUNDAY      7 // SONNTAG

#define BIT_HOUR_00     BIT_HOUR_24
#define BIT_HOUR_01     8
#define BIT_HOUR_02     9
#define BIT_HOUR_03     10
#define BIT_HOUR_04     11
#define BIT_HOUR_05     12
#define BIT_HOUR_06     13
#define BIT_HOUR_07     14
#define BIT_HOUR_08     15
#define BIT_HOUR_09     16
#define BIT_HOUR_10     17
#define BIT_HOUR_11     18
#define BIT_HOUR_12     19
#define BIT_HOUR_13     20
#define BIT_HOUR_14     21
#define BIT_HOUR_15     22
#define BIT_HOUR_16     23
#define BIT_HOUR_17     24
#define BIT_HOUR_18     25
#define BIT_HOUR_19     26
#define BIT_HOUR_20     27
#define BIT_HOUR_21     28
#define BIT_HOUR_22     29
#define BIT_HOUR_23     30
#define BIT_HOUR_24     31

#define MASK_ISACTIVE   B1

#define MASK_MONDAY     nt(B1<<BIT_MONDAY   )
#define MASK_TUESDAY    nt(B1<<BIT_TUESDAY  )
#define MASK_WEDNESDAY  nt(B1<<BIT_WEDNESDAY)
#define MASK_THURSDAY   nt(B1<<BIT_THURSDAY )
#define MASK_FRIDAY     nt(B1<<BIT_FRIDAY   )
#define MASK_SATURDAY   nt(B1<<BIT_SATURDAY )
#define MASK_SUNDAY     nt(B1<<BIT_SUNDAY   )

#define MASK_HOUR_00    MASK_HOUR_24
#define MASK_HOUR_01    int(B1<<BIT_HOUR_01)
#define MASK_HOUR_02    int(B1<<BIT_HOUR_02)
#define MASK_HOUR_03    int(B1<<BIT_HOUR_03)
#define MASK_HOUR_04    int(B1<<BIT_HOUR_04)
#define MASK_HOUR_05    int(B1<<BIT_HOUR_05)
#define MASK_HOUR_06    int(B1<<BIT_HOUR_06)
#define MASK_HOUR_07    int(B1<<BIT_HOUR_07)
#define MASK_HOUR_08    int(B1<<BIT_HOUR_08)
#define MASK_HOUR_09    int(B1<<BIT_HOUR_09)
#define MASK_HOUR_10    int(B1<<BIT_HOUR_10)
#define MASK_HOUR_11    int(B1<<BIT_HOUR_11)
#define MASK_HOUR_12    int(B1<<BIT_HOUR_12)
#define MASK_HOUR_13    int(B1<<BIT_HOUR_13)
#define MASK_HOUR_14    int(B1<<BIT_HOUR_14)
#define MASK_HOUR_15    int(B1<<BIT_HOUR_15)
#define MASK_HOUR_16    int(B1<<BIT_HOUR_16)
#define MASK_HOUR_17    int(B1<<BIT_HOUR_17)
#define MASK_HOUR_18    int(B1<<BIT_HOUR_18)
#define MASK_HOUR_19    int(B1<<BIT_HOUR_19)
#define MASK_HOUR_20    int(B1<<BIT_HOUR_20)
#define MASK_HOUR_21    int(B1<<BIT_HOUR_21)
#define MASK_HOUR_22    int(B1<<BIT_HOUR_22)
#define MASK_HOUR_23    int(B1<<BIT_HOUR_23)
#define MASK_HOUR_24    int(B1<<BIT_HOUR_24)


class Schedule : public Connector {
  
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
