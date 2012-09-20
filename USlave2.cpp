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
    for (uint8_t i=0; i < Core2::digitals_len; i++) {
      statusLine(Core2::digitals[i], (uint8_t) Core2::digitals[i].getValue());
    }
    // pulses
    Core2::copyToBuffer_P(PSTR("\n=== PULSES ===\n"));
    LOG("track2: ");LOGLN(Core2::pulses[0].getValue());
    for (uint8_t i=0; i < Core2::pulses_len; i++) {
      statusLine(Core2::pulses[i], Core2::pulses[i].getValue());
    }
    /*
    // messages
    Core2::copyToBuffer_P(PSTR("\n=== MESSAGES ===\n"));
    for (uint8_t i=0; i < Core2::messages_len; i++) {
      statusLine(Core2::messages[i], Core2::messages[i].getValue());
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
  Core2::copyToBuffer((uint8_t)value);
  Core2::copyToBuffer('\t');
  Core2::copyToBuffer_P(connector.getLabel());
  Core2::copyToBuffer('\n');
}

