#include "LSlave.h"



namespace LSlave {
  
  void begin()
  {
    _lcd.begin(LCD_WIDTH, LCD_HEIGHT);
  #if LCD_BLPIN
    pinMode(LCD_BLPIN, OUTPUT);
    digitalWrite(LCD_BLPIN, HIGH);
  #endif
    LOGLN("display LCD");
  }
  
  
  void check()
  {
    if (Serial.available()) {
      LOGLN(">>> LCD");
      LOGLN("<<< LCD");
    }
  }
  
  
  void uncheck()
  {
  }
  
  
  const Key getKey()
  {
    return KEYPAD_NONE;
    /*
    static int NUM_KEYS=5; // keys_len
    static int adc_key_val[5] ={  
      30, 150, 360, 535, 760     };
      
    int input = analogRead(LCD_ABTN);
    //static const prog_uchar lcd_keys[] PROGMEM
    // pgm_read_byte
    //while (i 
    for (uint8_t k=0; k<keys_len; k++)
    {
      if (input < pgm_read_byte(&lcd_keys[k]))
      {
        return k;
      }
    }
    if (k >= keys_len)
      k = -1;     // No valid key pressed
    return k;
    */
  }

}
