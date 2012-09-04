#ifndef FASTTIMER_H_
#define FASTTIMER_H_

#include "config.h"
#include "macros.h"
#include <Arduino.h>


//==>  >> 0: 1 unit of embedTime is 0.001s
//-->  const unsigned long maxTime = 4294967295; // = 49d 17h 02m 47s
//-->  const unsigned int maxTime = 65535; // = 65s
//==>  >> 12: 1 unit of embedTime is 4.096s
//-->  const unsigned int maxEmbedTime = 65535; // = 3d 02h 33mn 51s 360ms
//     const unsigned int moduloEmbedTime = 63281; // = 3d 00h 00mn 00s 000ms
//-->  const uint8_t maxEmbedTime = 255; // = 17mn 24s 480ms
//     const uint8_t moduloEmbedTime = 219; // = 15mn 00s 000ms
//==>  >> 14: 1 unit of embedTime is 16.384s
//-->  const uint8_t maxEmbedTime = 255; // = 1h 09mn 37s 920ms
//     const uint8_t moduloEmbedTime = 219; // = 1h 00mn 00s 000ms

/** located in macros.h, config.h define EMBEDTIME as:
#define EMBEDTIME_4s_15m   ((uint8_t) (millis() >> 12))
#define EMBEDTIME_4s_3d    ((unsigned int) (millis() >> 12))
#define EMBEDTIME_16s_1h   ((uint8_t) (millis() >> 14))
#define EMBEDTIME_65s_4h   ((uint8_t) (millis() >> 16))
**/


class FastTimer {
  public:
    // call it in the main loop()
    static boolean update();
    // call it in the main loop() if update() is true
    static boolean isNewCycle();
  private:
    static uint8_t _embedTime, _lastEmbedTime;
};


#endif FASTTIMER_H_

