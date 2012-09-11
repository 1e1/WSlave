#include "USlave.h"



namespace USlave {
  
  void begin()
  {
    Serial.begin(USB_SPEED);
  }
  
  
  void check()
  {
    if (Serial.available()) {
      LOGLN(">>> USB");
      Core::readLine(&Serial);
      _sendStatus();
      _close:
      // close the connection:
      Serial.flush();
      LOGLN("<<< USB");
    }
  }
  
  void _sendStatus()
  {
    char pin[2];
    Core_unbuffer();
    // digitals
    Core::_copyToBuffer(PSTR("\n=== DIGITALS ===\n"));
    for (uint8_t i=0; i < Core::digitals_len; i++) {
      _statusLine(DIGITAL_PIN_AT(i), DIGITAL_VALUE_AT(i), DIGITAL_LABEL_AT(i));
    }
    // pulses
    Core::_copyToBuffer(PSTR("\n=== PULSES ===\n"));
    for (uint8_t i=0; i < Core::pulses_len; i++) {
      _statusLine(PULSE_PIN_AT(i), PULSE_VALUE_AT(i), PULSE_LABEL_AT(i));
    }
    // messages
    Core::_copyToBuffer(PSTR("\n=== MESSAGES ===\n"));
    for (uint8_t i=0; i < Core::messages_len; i++) {
      Core::_copyToBuffer(Core::messages[i].label);
      Core::_copyToBuffer('\t');
      Core::_copyToBuffer(Core::messages[i].value);
      Core::_copyToBuffer('\n');
    }
    Core::_sendBuffer();
  }
  
  
  void _statusLine(uint8_t pin, uint8_t value, const char *label)
  {
    char cpin[2];
    Core::pinToChars(pin, cpin);
    Core::_copyToBuffer(label);
    Core::_copyToBuffer('\t');
    Core::_copyToBuffer(cpin);
    Core::_copyToBuffer('=');
    Core::_copyToBuffer(value);
    Core::_copyToBuffer('\n');
  }

}
