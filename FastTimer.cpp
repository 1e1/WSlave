#include "FastTimer.h"


namespace FastTimer {
  
  /** 
    *   0: same time section
    *   1: new time section
    * 255: new time section, new cycle
    * 
    * @return int8_t
    */
  const uint8_t update()
  {
    const uint8_t previousTime = _embedTime;
    //      LOG("("); LOG(_embedTime); LOG(") ");
    _embedTime = EMBEDTIME;
    //      LOG(_embedTime); LOG(" - "); LOG(previousTime); LOG(" = ");
    //      LOGLN((uint8_t)(_embedTime ^ previousTime));
    //      WAIT(5000);
    return _embedTime ^ previousTime;
  }
  
}

