#include "WSlave2.h"




/***********************************************************
 *                       PROPERTIES                        *
 **********************************************************/




EthernetServer WSlave2::_server(PORT);
EthernetClient WSlave2::_client;

LONGSTRING(header_200)    = "200 OK";
LONGSTRING(header_417)    = "417 Expectation failed";
LONGSTRING(header_text)   = "text/plain" CRLF;
LONGSTRING(header_json)   = "application/json" CRLF;
LONGSTRING(header_htZ)    = "text/html" CRLF "Content-Encoding: gzip";
LONGBYTES(webpage)        = WEBPAGE;
static size_t webpage_len = ARRAYLEN(webpage); // ~ 1557o / 1600o / 1709o / 2100o




/***********************************************************
 *                         PUBLIC                          *
 **********************************************************/




void WSlave2::begin()
{
  byte mac[] = { MAC };
  const IPAddress ip(IP);
  /*
  const IPAddress _gateway(GATEWAY);
  const IPAddress _subnet(SUBNET);
  */
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


void WSlave2::check()
{
  if (_client = _server.available()) {
    LOGLN(">>> ETH0");
    
    MethodType method = INVALID;
    ActionType action = ROOT;
    uint8_t watchdog  = MAXHEADERS;
    
    Core2::setStream(&_client);
    
    // Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
    Core2::readUntil(SP);
    if (Core2::bufferIsEqualTo_P(PSTR("GET"))) {
      LOG("GET ");
      method = GET;
    } else if (Core2::bufferIsEqualTo_P(PSTR("PUT"))) {
      LOG("PUT ");
      method = PUT;
    } else goto _send;
    
    Core2::readUntil(SP);
    if (Core2::bufferIsPrefixOf_P(PSTR("/ws"))) {
      action = SERVICE;
      LOGLN("webservice");
      // TODO catch /ws?param!!!
    } else if (Core2::bufferIsPrefixOf_P(PSTR("/dict"))) {
      action = DICTIONARY;
      LOGLN("dictionary");
    } else {
      LOGLN("*");
    }
    lineLength(); // ends first Header line
    
    // sweep headers until CRLF CRLF
    _crlfcrlf:
//      while (_nextHttpLine() && --watchdog);
    while (lineLength()>1 && --watchdog);
    if (!watchdog) {
      LOGLN("INVALID");
      method = INVALID;
    }
    
    // on body:
    if (method == PUT && action == SERVICE) {
      LOGLN("reading body");
      // [0-9]+=[0-9]+(&[0-9]+=[0-9]+)*
      Core2::processLine();
    }
    
    _send:
    if (method == INVALID) {
      sendHeaders_P(header_417, header_text);
    } else {
      switch (action) {
        case SERVICE:
        sendHeaders_P(header_200, header_json);
        LOGLN("< send service");
        sendService();
        break;
        case DICTIONARY:
        sendHeaders_P(header_200, header_json);
        LOGLN("< send dictionnary");
        sendDictionary();
        break;
        default:
        LOG("< webpage_len="); LOGLN(webpage_len);
        sendHeaders_P(header_200, header_htZ);
        sendDefault_P(webpage, webpage_len);
      } // switch (action)
    } // else (method == INVALID)
    LOGLN("<<< ETH0");
  } // if (_client = _server.available())
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
void WSlave2::sendHeaders_P(const prog_char *codeStatus, const prog_char *contentType)
{
  Core2::unbuffer();
  Core2::copyToBuffer_P(PSTR("HTTP/1.1 "));
  Core2::copyToBuffer_P(codeStatus);
  Core2::copyToBuffer_P(PSTR(CRLF "Content-Type: "));
  Core2::copyToBuffer_P(contentType);
  //Core2::copyToBuffer_P(PSTR(CRLF "Connection: close"));
  Core2::copyToBuffer(CRLF);
  Core2::sendBuffer();
}


void WSlave2::sendDictionary()
{
  uint8_t coma = STATIC_TOTAL_LEN;
  Core2::unbuffer();
  Core2::copyToBuffer('{');
  /*
  // messages
  for (uint8_t i=0; i < STATIC_MESSAGES_LEN; i++) {
    sendToJson('M', STATIC_MESSAGES[i], --coma);
  }
  */
  // pulses
  for (uint8_t i=0; i < STATIC_PULSES_LEN; i++) {
    sendToJson('M', STATIC_PULSES[i], --coma);
  }
  // digitals
  for (uint8_t i=0; i < STATIC_DIGITALS_LEN; i++) {
    sendToJson('M', STATIC_DIGITALS[i], --coma);
  }
  Core2::copyToBuffer('}');
  Core2::sendBuffer();
}


void WSlave2::sendService()
{
  uint8_t coma = STATIC_TOTAL_LEN;
  Core2::unbuffer();
  Core2::copyToBuffer('[');
  /*
  // messages
  for (uint8_t i=0; i < STATIC_MESSAGES_LEN; i++) {
    Core2::copyToBuffer(STATIC_MESSAGES[i].value);
    if (--coma) {
      Core2::copyToBuffer(',');
    }
  }
  */
  // pulses
  for (uint8_t i=0; i < STATIC_PULSES_LEN; i++) {
    Core2::copyToBuffer(STATIC_PULSES[i].getValue());
    if (--coma) {
      Core2::copyToBuffer(',');
    }
  }
  // digitals
  for (uint8_t i=0; i < STATIC_DIGITALS_LEN; i++) {
    Core2::copyToBuffer(STATIC_DIGITALS[i].getValue());
    if (--coma) {
      Core2::copyToBuffer(',');
    }
  }
  Core2::copyToBuffer(']');
  Core2::sendBuffer();
}


void WSlave2::sendDefault_P(const prog_uchar *data, size_t length)
{
  Core2::copyToBuffer_P(data, length);
  Core2::sendBuffer();
}


/** 
  * copy the string starting here until the end character
  * into buffer (reduce the bufferSiez)
  *
  * @return false if end by a new line
  */
const boolean WSlave2::nextHttpLine()
{
  uint8_t watchdog = MAXLINESIZE;
  char c;
  _carriageReturn:
  while ((c=_client.read())!=CR && --watchdog && c!=-1);
  _lineFeed:
  if (watchdog && (c=_client.read())!=LF && c!=-1) {
    goto _carriageReturn;
  }
  return watchdog != MAXLINESIZE;
}


const uint8_t WSlave2::lineLength()
{
  uint8_t watchdog = MAXLINESIZE;
  char c;
  while ((c=_client.read())!=LF && --watchdog && c!=-1);
  LOG("header length: "); LOGLN(MAXLINESIZE - watchdog);
  return MAXLINESIZE - watchdog;
}


void WSlave2::sendToJson(const char type, Connector connector, const uint8_t coma)
{
  char pinChars[4] = { type, '0'+(connector.getPin()/10), '0'+(connector.getPin()%10), '\0' };
  Core2::copyToBuffer('"');
  Core2::copyToBuffer(connector.getPin());
  Core2::copyToBuffer("\":\"");
  Core2::copyToBuffer_P(connector.getLabel());
  if (coma) {
    Core2::copyToBuffer("\",");
  } else {
    Core2::copyToBuffer('"');
  }
}
