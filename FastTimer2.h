#ifndef FASTTIMER2_H_
#define FASTTIMER2_H_

#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUDP.h>
#include "config.h"
#include "macros.h"
#include "Core2.h"


//==>  >> 0: 1 unit of embedTime is 0.001s
//-->  const unsigned long maxTime        = 4294967295; // = 49d 17h 02m 47s
//-->  const unsigned int maxTime         = 65535;      // = 65s
//==>  >> 10: 1 unit of embedTime is 1.024s
//-->  const uint8_t maxEmbedTime         = 255;        // = 4mn 21s 120ms
//==>  >> 12: 1 unit of embedTime is 4.096s
//-->  const unsigned int maxEmbedTime    = 65535;      // = 3d 02h 33mn 51s 360ms
//     const unsigned int moduloEmbedTime = 63281;      // = 3d 00h 00mn 00s 000ms
//-->  const uint8_t maxEmbedTime         = 255;        // = 17mn 24s 480ms
//     const uint8_t moduloEmbedTime      = 219;        // = 15mn 00s 000ms
//==>  >> 14: 1 unit of embedTime is 16.384s
//-->  const uint8_t maxEmbedTime         = 255;        // = 1h 09mn 37s 920ms
//     const uint8_t moduloEmbedTime      = 219;        // = 1h 00mn 00s 000ms
//==>  >> 16: 1 unit of embedTime is 65.536s
//-->  const uint8_t maxEmbedTime         = 255;        // = 4h 38mn 31s 680ms
//     const uint8_t moduloEmbedTime      = 219;        // = 4h 00mn 00s 000ms

/** located in macros.h, config.h define EMBEDTIME as:
#define EMBEDTIME_1s_4m    ((uint8_t) (millis() >> 10))
#define EMBEDTIME_4s_15m   ((uint8_t) (millis() >> 12))
#define EMBEDTIME_4s_3d    ((unsigned int) (millis() >> 12))
#define EMBEDTIME_16s_1h   ((uint8_t) (millis() >> 14))
#define EMBEDTIME_65s_4h   ((uint8_t) (millis() >> 16))
**/

// SDDD HHHH (DST[0..1], day[0..6], hour[0..23])
#define FMASK_DST(dst)        byte(dst << 7)
#define FMASK_DAY(dayOfWeek)  byte(dayOfWeek << 3)
#define FMASK_HOUR(hour)      byte(hour)


class FastTimer2 {
  
  public:
  static void begin(); // call it in setup()
  static const uint8_t update(); // call it once in the main loop()
  static void requestNtp();
  static void readNtp();
  
  protected:
  static EthernetUDP _server;
  static IPAddress _timeServer;
  static uint8_t _embedTime;
  static uint8_t _referenceTime; // SDDD HHHH (DST[0..1], day[0..6], hour[0..23])
  
};


#endif FASTTIMER2_H_

