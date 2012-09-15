#include "USlave.h"



namespace USlave {
  
  void begin()
  {
    Serial.begin(USB_SPEED);
    LOGLN("listen USB");
  }
  
  
  void check()
  {
    if (Serial.available()) {
      LOGLN(">>> USB");
      Core::setStream(&Serial);
      Core::processLine();
      _sendStatus();
      LOGLN("<<< USB");
    }
  }
  
  
  void uncheck()
  {
    // close the connection:
    Serial.flush();
  }
  
  
  void _sendStatus()
  {
    Core::_unbuffer();
    // digitals
    Core::_copyToBuffer_P(PSTR("\n=== DIGITALS ===\n"));
    for (uint8_t i=0; i < Core::digitals_len; i++) {
      _statusLine(DIGITAL_PIN_AT(i), Core::getDigitalValueAtIndex(i), DIGITAL_LABEL_AT(i));
    }
    // pulses
    Core::_copyToBuffer_P(PSTR("\n=== PULSES ===\n"));
    for (uint8_t i=0; i < Core::pulses_len; i++) {
      _statusLine(PULSE_PIN_AT(i), Core::getPulseValueAtIndex(i), PULSE_LABEL_AT(i));
    }
    // messages
    Core::_copyToBuffer_P(PSTR("\n=== MESSAGES ===\n"));
    for (uint8_t i=0; i < Core::messages_len; i++) {
      Core::_copyToBuffer(Core::messages[i].label);
      Core::_copyToBuffer('\t');
      Core::_copyToBuffer(Core::messages[i].value);
      Core::_copyToBuffer('\n');
    }
    Core::_copyToBuffer('\n');
    Core::_sendBuffer();
  }
  
  
  void _statusLine(uint8_t pin, uint8_t value, const char *label)
  {
    using Core::_copyToBuffer;
    _copyToBuffer(pin);
    _copyToBuffer('\t');
    _copyToBuffer(value);
    _copyToBuffer('\t');
    _copyToBuffer(label);
    _copyToBuffer('\n');
  }

}
