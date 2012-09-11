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
    _server.begin();
  }
  
  
  void check()
  {
    if (_client = _server.available()) {
      LOGLN(">>> ETH0");
      
      MethodType method = INVALID;
      ActionType action = ROOT;
      uint8_t watchdog  = MAXHEADERS;
      
      // Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
      //_scanHttpLine(SP);
      Core::readLine(&_client, SP);
      if (Core::_bufferIsEqualTo(PSTR("GET"))) {
        LOG("GET ");
        method = GET;
      } else if (Core::_bufferIsEqualTo(PSTR("PUT"))) {
        LOG("PUT ");
        method = PUT;
      } else goto _send;
      
      Core::readLine(&_client, SP);
      if (Core::_bufferIsPrefixOf(PSTR("/ws"))) {
        action = SERVICE;
        LOGLN("webservice");
        // TODO catch /ws?param!!!
      } else if (Core::_bufferIsPrefixOf(PSTR("/dict"))) {
        action = DICTIONARY;
        LOGLN("dictionary");
      } else {
        LOGLN("*");
      }
      _nextHttpLine();
      
      // sweep headers until CRLF CRLF
      //LOGLN("sweeping headers");
      _crlfcrlf:
      while (_nextHttpLine() && --watchdog);
      //if (_nextHttpLine() && watchdog) goto _crlfcrlf;
      if (!watchdog) {
        method = INVALID;
      }
      //LOGLN("ready to read body");
      
      // on body:
      if (method == PUT && action == SERVICE) {
        LOGLN("TODO: reading body");
        // [0-9]+=[0-9]+(&[0-9]+=[0-9]+)*
        Core::readLine(&_client);
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
        }
      }
      
      // give the web browser time to receive the data
      delay(1);
      _close:
      // close the connection:
      _client.flush();
      _client.stop();
      LOGLN("<<< ETH0");
    }
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
  void _sendHeaders_P(const prog_uchar *codeStatus, const prog_uchar *contentType)
  {
    Core_unbuffer();
    Core::_copyToBuffer(PSTR("HTTP/1.1 "));
    LOG("-");
    Core::_copyToBuffer_P(codeStatus);
    LOG("-");
    Core::_copyToBuffer(PSTR(CRLF "Content-Type: "));
    LOG("-");
    Core::_copyToBuffer_P(contentType);
    LOG("-");
    //Core::_copyToBuffer(PSTR(CRLF "Connection: close"));
    LOG("-");
    Core::_copyToBuffer(CRLF);
    LOG("-");
    Core::_sendBuffer();
  }
  
  
  void _sendDictionary()
  {
    //const char *strings[Core::messages_len + Core::pulses_len + Core::digitals_len];
    char pinChars[2];
    Core_unbuffer();
    Core::_copyToBuffer('{');
    // messages
    for (uint8_t i=0; i < Core::messages_len; i++) {
      Core::pinToChars(i, pinChars);
      Core::_copyJsonToBuffer('M', pinChars, Core::messages[i].label);
    }
    // pulses
    for (uint8_t i=0; i < Core::pulses_len; i++) {
      Core::pinToChars(Core::pulses[i].pin, pinChars);
      Core::_copyJsonToBuffer('P', pinChars, Core::pulses[i].label);
    }
    // digitals
    for (uint8_t i=0; i < Core::digitals_len; i++) {
      Core::pinToChars(Core::digitals[i].vPin, pinChars);
      Core::_copyJsonToBuffer('D', pinChars, Core::digitals[i].label);
    }
    Core::_copyToBuffer("\"M#\":\"FastTimer\"}");
    Core::_sendBuffer();
  }
  
  
  void _sendService()
  {
    Core_unbuffer();
    Core::_copyToBuffer('[');
    // messages
    for (uint8_t i=0; i < Core::messages_len; i++) {
      Core::_copyToBuffer(Core::messages[i].value);
      Core::_copyToBuffer(',');
    }
    // pulses
    for (uint8_t i=0; i < Core::pulses_len; i++) {
      Core::_copyToBuffer(PULSE_VALUE_AT(i));
      Core::_copyToBuffer(',');
    }
    // digitals
    for (uint8_t i=0; i < Core::digitals_len; i++) {
      Core::_copyToBuffer((uint8_t)DIGITAL_VALUE_AT(i));
      Core::_copyToBuffer(',');
    }
    Core::_copyToBuffer("\"#\"");
    Core::_copyToBuffer(']');
    Core::_sendBuffer();
  }
  
  
  void _sendDefault_P(const prog_uchar *data, size_t length)
  {
    Core_unbuffer();
    while (length--) {
      Core::_copyToBuffer(pgm_read_byte(data++));
    }
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
    int c;
    uint8_t watchdog = MAXLINESIZE;
    _carriageReturn:
    while (_client.connected() && _client.available() && _client.read() != CR && --watchdog);
    _lineFeed:
    if (_client.connected() && _client.available() && _client.read() != LF && watchdog) {
      goto _carriageReturn;
    }
    return watchdog != MAXLINESIZE;
  }

}
