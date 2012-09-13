#include "WSlave.h"



namespace WSlave {
  
  void begin()
  {
  #if USE_DHCP
    LOGLN("Trying to get an IP address using DHCP");
    if (0==Ethernet.begin(mac)) {
      LOGLN("Failed to configure Ethernet using DHCP");
  #endif
      Ethernet.begin(mac, ip/*, {DNS}, gateway, subnet*/);
  #if USE_DHCP
    }
  #endif
    // then don't forget Ethernet.maintain()
    LOG("IP:   ");  LOGLN(Ethernet.localIP());
    LOG("MASK: ");  LOGLN(Ethernet.subnetMask());
    LOG("GATE: ");  LOGLN(Ethernet.gatewayIP());
    LOG("DNS:  ");  LOGLN(Ethernet.dnsServerIP());
    LOG("listen "); LOGLN(PORT);
    _server.begin();
  }
  
  
  void check()
  {
    if (_client = _server.available()) {
      LOGLN(">>> ETH0");
      
      MethodType method = INVALID;
      ActionType action = ROOT;
      uint8_t watchdog  = MAXHEADERS;
      
      Core::setStream(&_client);
      
      // Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
      Core::readUntil(SP);
      if (Core::_bufferIsEqualTo_P(PSTR("GET"))) {
        LOG("GET ");
        method = GET;
      } else if (Core::_bufferIsEqualTo_P(PSTR("PUT"))) {
        LOG("PUT ");
        method = PUT;
      } else goto _send;
      
      Core::readUntil(SP);
      if (Core::_bufferIsPrefixOf_P(PSTR("/ws"))) {
        action = SERVICE;
        LOGLN("webservice");
        // TODO catch /ws?param!!!
      } else if (Core::_bufferIsPrefixOf_P(PSTR("/dict"))) {
        action = DICTIONARY;
        LOGLN("dictionary");
      } else {
        LOGLN("*");
      }
      _lineLength(); // ends first Header line
      
      // sweep headers until CRLF CRLF
      _crlfcrlf:
      while (_lineLength() && --watchdog);
      if (!watchdog) {
        LOGLN("INVALID");
        method = INVALID;
      }
      
      // on body:
      if (method == PUT && action == SERVICE) {
        LOGLN("reading body");
        // [0-9]+=[0-9]+(&[0-9]+=[0-9]+)*
        Core::processLine();
      }
      
      _send:
      if (method == INVALID) {
        _sendHeaders_P(header_417, header_text);
      } else {
        switch (action) {
          case SERVICE:
          _sendHeaders_P(header_200, header_json);
          LOGLN("< send service");
          _sendService();
          break;
          case DICTIONARY:
          _sendHeaders_P(header_200, header_json);
          LOGLN("< send dictionnary");
          _sendDictionary();
          break;
          default:
          LOG("< webpage_len="); LOGLN(webpage_len);
          _sendHeaders_P(header_200, header_htZ);
          _sendDefault_P(webpage, webpage_len);
        } // switch (action)
      } // else (method == INVALID)
      LOGLN("<<< ETH0");
    } // if (_client = _server.available())
  }
  
  
  void uncheck()
  {
    //_client.flush();
    _client.stop();
  }
  
  
  void maintain()
  {
  #if USE_DHCP
    Ethernet.maintain(); /* added in 1.0.1 - default Ubuntu IDE is still in 1.0 */
    LOGLN("renew DHCP");
  #endif
  }
  
  
  /** 
    * Status:
    *   1: 200
    *   2: 200
    *   3: 200
    *   *: 400 "Bad Request"
    *   *: 417 "The behavior expected fot the server is not supported."
    * Content-Type:
    *   1: application/json
    *   2: text/html
    *   3: text/cache-manifest
    * Content-Encoding:
    *   2: gzip
    * Cache-Control:
    *   2: max-age=604800 // 7* 24* 60* 60
    * Connection: close
    */
  void _sendHeaders_P(const prog_char *codeStatus, const prog_char *contentType)
  {
    Core::_unbuffer();
    Core::_copyToBuffer_P(PSTR("HTTP/1.1 "));
    Core::_copyToBuffer_P(codeStatus);
    Core::_copyToBuffer_P(PSTR(CRLF "Content-Type: "));
    Core::_copyToBuffer_P(contentType);
    //Core::_copyToBuffer_P(PSTR(CRLF "Connection: close"));
    Core::_copyToBuffer(CRLF);
    Core::_sendBuffer();
  }
  
  
  void _sendDictionary()
  {
    uint8_t coma = Core::total_len;
    Core::_unbuffer();
    Core::_copyToBuffer('{');
    // messages
    for (uint8_t i=0; i < Core::messages_len; i++) {
      _sendToJson(i, 'M', MESSAGE_LABEL_AT(i), --coma);
    }
    // pulses
    for (uint8_t i=0; i < Core::pulses_len; i++) {
      _sendToJson(PULSE_PIN_AT(i), 'P', PULSE_LABEL_AT(i), --coma);
    }
    // digitals
    for (uint8_t i=0; i < Core::digitals_len; i++) {
      _sendToJson(DIGITAL_PIN_AT(i), 'D', DIGITAL_LABEL_AT(i), --coma);
    }
    Core::_copyToBuffer('}');
    Core::_sendBuffer();
  }
  
  
  void _sendService()
  {
    uint8_t coma = Core::total_len;
    Core::_unbuffer();
    Core::_copyToBuffer('[');
    // messages
    for (uint8_t i=0; i < Core::messages_len; i++) {
      Core::_copyToBuffer(Core::messages[i].value);
      if (--coma) {
        Core::_copyToBuffer(',');
      }
    }
    // pulses
    for (uint8_t i=0; i < Core::pulses_len; i++) {
      Core::_copyToBuffer(PULSE_VALUE_AT(i));
      if (--coma) {
        Core::_copyToBuffer(',');
      }
    }
    // digitals
    for (uint8_t i=0; i < Core::digitals_len; i++) {
      Core::_copyToBuffer((uint8_t)DIGITAL_VALUE_AT(i));
      if (--coma) {
        Core::_copyToBuffer(',');
      }
    }
    Core::_copyToBuffer(']');
    Core::_sendBuffer();
  }
  
  
  void _sendDefault_P(const prog_uchar *data, size_t length)
  {
    Core::_copyToBuffer_P(data, length);
    Core::_sendBuffer();
  }
  
  
  /** 
    * copy the string starting here until the end character
    * into buffer (reduce the bufferSiez)
    *
    * @return false if end by a new line
    */
  const boolean _nextHttpLine()
  {
    uint8_t watchdog = MAXLINESIZE;
    _carriageReturn:
    while (_client.available() && _client.read() != CR && --watchdog);
    _lineFeed:
    if (watchdog && _client.available() && _client.read() != LF) {
      goto _carriageReturn;
    }
    return watchdog != MAXLINESIZE;
  }
  
  
  const uint8_t _lineLength()
  {
    uint8_t watchdog = MAXLINESIZE;
    while (_client.available() && _client.read() != LF && --watchdog);
    return MAXLINESIZE - watchdog;
  }
  
  
  void _sendToJson(const uint8_t pin, const char type, const char *label, const uint8_t coma)
  {
    char pinChars[4] = { type, '0'+(pin/10), '0'+(pin%10), '\0' };
    Core::_copyJsonToBuffer(pinChars, label, coma);
  }

}
