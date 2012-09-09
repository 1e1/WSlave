#include "Core.h"



namespace Core {
  
  /** 
    * pin number MUST be in 0..99
    */
  void pinToChars(uint8_t pin, char out[2])
  {
    out[1] = '0' + (pin % 10);
    if (pin > 9) {
      out[0] = '0' + (pin / 10);
    } else {
      out[0] = '0';
    }
    // LOG(pin); LOG(": "); LOG(out[0]); LOG("; "); LOG(out[1]); LOGLN("; ");
  }
  
}