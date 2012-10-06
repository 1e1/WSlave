#include "WSlave2.h"




/***********************************************************
 *                       PROPERTIES                        *
 **********************************************************/




EthernetServer WSlave2::_server(PORT);
EthernetClient WSlave2::_client;

LONGSTRING(header_200)    = "200 OK";
LONGSTRING(header_401)    = "401 Authorization Required" CRLF "WWW-Authenticate: Basic realm=\"" DEVICE_NAME "\"";
LONGSTRING(header_417)    = "417 Expectation failed";
LONGSTRING(header_text)   = "text/plain";
LONGSTRING(header_json)   = "application/json";
LONGSTRING(header_htZ)    = "text/html" CRLF "Content-Encoding: gzip";
LONGSTRING(header_end)    = CRLF CRLF;
LONGSTRING(crlf)          = CRLF;

LONGSTRING(email)         = EMAIL;

LONGBYTES(webpage)        = WEBPAGE;
static size_t webpage_len = ARRAYLEN(webpage); // ~ 1557o / 1600o / 1709o / 2100o

LONGSTRING(json_qcolon)   = "\":\"";
LONGSTRING(json_qcomma)   = "\",\"";
LONGSTRING(json_qbrace1)  = "{\"";
LONGSTRING(json_qbrace2)  = "\"}";




/***********************************************************
 *                         PUBLIC                          *
 **********************************************************/




void WSlave2::begin()
{
  byte mac[] = { MAC };
  const IPAddress ip(IP);
  /*
  const IPAddress dns(DNS);
  const IPAddress gateway(GATEWAY);
  const IPAddress subnet(SUBNET);
  */
#if USE_DHCP
  LOGLN("Trying to get an IP address using DHCP");
  if (0==Ethernet.begin(mac)) {
    LOGLN("Failed to configure Ethernet using DHCP");
#endif
    Ethernet.begin(mac, ip/*, dns, gateway, subnet*/);
#if USE_DHCP
  }
#endif
  // then don't forget Ethernet.maintain()
  LOG("IP:   ");  LOGLN(Ethernet.localIP());
  LOG("MASK: ");  LOGLN(Ethernet.subnetMask());
  LOG("GATE: ");  LOGLN(Ethernet.gatewayIP());
  LOG("DNS:  ");  LOGLN(Ethernet.dnsServerIP());
  LOG("listen "); LOGLN(PORT);
  WSlave2::_server.begin();
  //delay(1000);
}


void WSlave2::check()
{
  if (WSlave2::_client = _server.available()) {
    LOGLN(">>> ETH0");
    
    MethodType method       = INVALID;
    ActionType action       = ROOT;
    uint8_t watchdog        = MAXHEADERS;
    boolean isUnauthorized  = true;
    
    Core2::setStream(&(WSlave2::_client));
    
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
    }
    WSlave2::lineLength(); // ends first Header line
    
    // check credentials = Authorization: Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==
    //header_401
    do {
      Core2::readUntil(SP);
      if (Core2::bufferIsEqualTo_P(PSTR("Authorization:"))) {
        Core2::readUntil(SP);
        if (Core2::bufferIsEqualTo_P(PSTR("Basic"))) {
          Core2::readUntil(CR);
          if (Core2::bufferIsEqualTo_P(PSTR(HTTP_AUTH64))) {
            isUnauthorized = false;
            WSlave2::lineLength(); // ends first Header line
            goto _crlfcrlf;
          }
        }
      }
    } while(isUnauthorized && WSlave2::lineLength()>1 && --watchdog);
    
    // sweep headers until CRLF CRLF
    _crlfcrlf:
//    while (WSlave2::nextHttpLine() && --watchdog);
    while (WSlave2::lineLength()>1 && --watchdog);
    if (!watchdog) {
      LOGLN("INVALID");
      method = INVALID;
    }
    
    // on body:
    if (method == PUT && action == SERVICE) {
      LOGLN("reading body");
      // [0-9]+=[0-9]+(&[0-9]+=[0-9]+)*
      Core2::processLine();
      //Core2::readUntil(CR);
      //Core2::printBuffer();
    }
    LOG("isUnauthorized="); LOGLN(isUnauthorized);
    LOG("watchdog="); LOGLN(watchdog);
    
    _send:
    if (isUnauthorized) {
      WSlave2::sendHeaders_P(header_401, header_text);
    } else {
      if (method == INVALID) {
        WSlave2::sendHeaders_P(header_417, header_text);
      } else {
        switch (action) {
          case SERVICE:
          WSlave2::sendHeaders_P(header_200, header_json);
          LOGLN("< send service");
          WSlave2::sendService();
          break;
          case DICTIONARY:
          WSlave2::sendHeaders_P(header_200, header_json);
          LOGLN("< send dictionnary");
          WSlave2::sendDictionary();
          break;
          default:
          LOG("< webpage_len="); LOGLN(webpage_len);
          WSlave2::sendHeaders_P(header_200, header_htZ);
          WSlave2::sendBody_P(webpage, webpage_len);
        } // switch (action)
      } // else (method == INVALID)
    } // else isUnauthorized
    LOGLN("<<< ETH0");
  } // if (WSlave2::_client = _server.available())
}


void WSlave2::sendEmail(const prog_char* sms, const uint8_t value)
{
  byte smtp[]       = { SMTP_IP };
  uint8_t watchdog  = MAXRETRIES;
  uint8_t state     = 6;
  Core2::setStream(&(WSlave2::_client));
  LOGLN("BEGIN");
  if (WSlave2::_client.connect(smtp, SMTP_PORT)) {
    do {
      WSlave2::waitClient(watchdog);
      LOG("< state #");
      switch (--state) {
        case 5:
        Core2::copyToBuffer_P(PSTR("HELO")); // HELO | EHLO
        break;
        case 4:
        Core2::copyToBuffer_P(PSTR("MAIL FROM:<"));
        Core2::copyToBuffer_P(email);
        Core2::copyToBuffer('>');
        break;
        case 3:
        Core2::copyToBuffer_P(PSTR("RCPT TO:<"));
        Core2::copyToBuffer_P(email);
        Core2::copyToBuffer('>');
        break;
        case 2:
        Core2::copyToBuffer_P(PSTR("DATA"));
        break;
        case 1:
        Core2::copyToBuffer_P(PSTR("Subject:" ML_SUBJECT CRLF CRLF));
        Core2::copyToBuffer_P(sms);
        Core2::copyToBuffer_P( crlf );
        Core2::copyToBuffer(value);
        Core2::copyToBuffer_P( crlf );
        Core2::copyToBuffer('.');
        break;
        case 0:
        Core2::copyToBuffer_P(PSTR("QUIT"));
        break;
      }
      //Core2::copyToBuffer( LF );
      Core2::copyToBuffer_P( crlf );
      Core2::sendBuffer();
      LOGLN(state);
    } while(watchdog && state);
    WSlave2::waitClient(watchdog);
    WSlave2::_client.stop();
  }
  LOGLN("END");
}




/***********************************************************
 *                        PROTECTED                        *
 **********************************************************/




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
void WSlave2::sendHeaders_P(const prog_char* codeStatus, const prog_char* contentType)
{
  Core2::unbuffer();
  Core2::copyToBuffer_P(PSTR("HTTP/1.1 "));
  Core2::copyToBuffer_P(codeStatus);
  Core2::copyToBuffer_P(PSTR(CRLF "Content-Type: "));
  Core2::copyToBuffer_P(contentType);
  //Core2::copyToBuffer_P(PSTR(CRLF "Connection: close"));
  Core2::copyToBuffer_P(header_end);
  Core2::sendBuffer();
}


void WSlave2::sendDictionary()
{
  uint8_t comma = Core2::total_len;
  Core2::unbuffer();
  Core2::copyToBuffer_P(json_qbrace1);
  // schedules
  for (uint8_t i=0; i < Core2::schedules_len; i++) {
    WSlave2::sendToJson('S', Core2::schedules[i], --comma);
  }
  // pulses
  for (uint8_t i=0; i < Core2::pulses_len; i++) {
    WSlave2::sendToJson('P', Core2::pulses[i], --comma);
  }
  // digitals
  for (uint8_t i=0; i < Core2::digitals_len; i++) {
    WSlave2::sendToJson('D', Core2::digitals[i], --comma);
  }
  /*
  // messages
  for (uint8_t i=0; i < Core2::messages_len; i++) {
    WSlave2::sendToJson('M', Core2::messages[i], --comma);
  }
  */
  Core2::copyToBuffer_P(json_qbrace2);
  Core2::sendBuffer();
}


void WSlave2::sendService()
{
  uint8_t comma = Core2::total_len;
  Core2::unbuffer();
  Core2::copyToBuffer('[');
  // schedules
  for (uint8_t i=0; i < Core2::schedules_len; i++) {
    Core2::copyToBuffer(Core2::schedules[i].getValue());
    if (--comma) {
      Core2::copyToBuffer(',');
    }
  }
  // pulses
  for (uint8_t i=0; i < Core2::pulses_len; i++) {
    Core2::copyToBuffer(Core2::pulses[i].getValue());
    if (--comma) {
      Core2::copyToBuffer(',');
    }
  }
  // digitals
  for (uint8_t i=0; i < Core2::digitals_len; i++) {
    Core2::copyToBuffer(Core2::digitals[i].getValue());
    if (--comma) {
      Core2::copyToBuffer(',');
    }
  }
  /*
  // messages
  for (uint8_t i=0; i < Core2::messages_len; i++) {
    Core2::copyToBuffer(Core2::messages[i].value);
    if (--comma) {
      Core2::copyToBuffer(',');
    }
  }
  */
  Core2::copyToBuffer(']');
  Core2::sendBuffer();
}


void WSlave2::sendBody_P(const prog_uchar *data, size_t length)
{
  Core2::copyToBuffer_P(data, length);
  Core2::sendBuffer();
  //WSlave2::_client.write(data, length);
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
  while ((c=WSlave2::_client.read())!=CR && --watchdog && c!=-1);
  _lineFeed:
  if (watchdog && (c=WSlave2::_client.read())!=LF && c!=-1) {
    goto _carriageReturn;
  }
  return watchdog != MAXLINESIZE;
}


const uint8_t WSlave2::lineLength()
{
  uint8_t watchdog = MAXLINESIZE;
  char c;
  while ((c=WSlave2::_client.read())!=LF && --watchdog && c!=-1);
  LOG("header length: "); LOGLN(MAXLINESIZE - watchdog);
  return MAXLINESIZE - watchdog;
}


void WSlave2::sendToJson(const char type, Connector connector, const boolean comma)
{
  char pinChars[3] = { type, '0'+(connector.getPin()/10), '0'+(connector.getPin()%10) };
  Core2::copyToBuffer(pinChars, 3);
  Core2::copyToBuffer_P(json_qcolon);
  Core2::copyToBuffer_P(connector.getLabel());
  if (comma) {
    Core2::copyToBuffer_P(json_qcomma);
  }
}


void WSlave2::waitClient(uint8_t& watchdog)
{
  do {
    delay(READCHAR_TIMEOUT);
  } while(!WSlave2::_client.available() && --watchdog);
  /* == * /
  char c;
  LOG("> ");
  while((c=WSlave2::_client.read()) && 32<=c && c<=127) {
    LOG(c);
  }
  LOGLN();
  /* == */
  WSlave2::_client.flush();
}
