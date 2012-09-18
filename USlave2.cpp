#include "USlave2.h"




/***********************************************************
 *                         PUBLIC                          *
 **********************************************************/




void USlave2::check()
{
  if (Serial.available()) {
    LOGLN(">>> USB");
    
    Core2::setStream(&Serial);
    Core2::processLine();
    
    Core2::unbuffer();
    // digitals
    Core2::copyToBuffer_P(PSTR("\n=== DIGITALS ===\n"));
    for (uint8_t i=0; i < STATIC_DIGITALS_LEN; i++) {
      statusLine(STATIC_DIGITALS[i], (uint8_t) STATIC_DIGITALS[i].getValue());
    }
    // pulses
    Core2::copyToBuffer_P(PSTR("\n=== PULSES ===\n"));
    for (uint8_t i=0; i < STATIC_PULSES_LEN; i++) {
      statusLine(STATIC_PULSES[i], STATIC_PULSES[i].getValue());
    }
    /*
    // messages
    Core2::copyToBuffer_P(PSTR("\n=== MESSAGES ===\n"));
    for (uint8_t i=0; i < STATIC_MESSAGES_LEN; i++) {
      statusLine(STATIC_MESSAGES[i], STATIC_MESSAGES[i].getValue());
    }
    */
    Core2::copyToBuffer('\n');
    Core2::sendBuffer();
    
    LOGLN("<<< USB");
  }
}




/***********************************************************
 *                        PROTECTED                        *
 **********************************************************/




void USlave2::statusLine(Connector connector, uint8_t value)
{
  Core2::copyToBuffer(connector.getPin());
  Core2::copyToBuffer('\t');
  Core2::copyToBuffer(value);
  Core2::copyToBuffer('\t');
  Core2::copyToBuffer_P(connector.getLabel());
  Core2::copyToBuffer('\n');
}

