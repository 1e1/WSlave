#include "WSlave.h"



WSlave::WSlave() :
  _server(PORT)/*,
  _client()*/
{
}


void WSlave::begin()
{
  _server.begin();
}


void WSlave::check()
{
  if (_client = _server.available()) {
    LOGLN(">>> new client");
    
    MethodType method = INVALID;
    ActionType action = ROOT;
    uint8_t watchdog  = MAXHEADERS;
    
    // Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
    //_scanHttpLine(SP);
    Core::_unbuffer();
    _client.readBytesUntil(SP, Core::_buffer, READBUFFERSIZE);
    LOG("method=");
    if (Core::_bufferIsEqualTo("GET")) {
      LOGLN("GET");
      method = GET;
    } else if (Core::_bufferIsEqualTo("PUT")) {
      LOGLN("PUT");
      method = PUT;
    } else goto _send;
    
    Core::_unbuffer();
    _client.readBytesUntil(SP, Core::_buffer, READBUFFERSIZE);
    LOG("action=");
    if (Core::_bufferIsPrefixOf("/ws")) {
      action = SERVICE;
      LOGLN("webservice");
      // TODO catch /ws?param!!!
    } else if (Core::_bufferIsPrefixOf("/dict")) {
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
      _sendHeaders("417 Expectation failed", "text/plain");
    } else {
      switch (action) {
        case SERVICE:
        _sendHeaders("200 OK", "application/json");
        LOGLN("< send service");
        _sendService();
        break;
        case DICTIONARY:
        _sendHeaders("200 OK", "application/json");
        LOGLN("< send dictionnary");
        _sendDictionary();
        break;
        default:
        LOG("< webpage_len="); LOGLN(webpage_len);
        _sendHeaders("200 OK", "text/html" CRLF "Content-Encoding: gzip");
        _sendDefault(webpage, webpage_len);
      }
    }
    
    // give the web browser time to receive the data
    delay(1);
    _close:
    // close the connection:
    _client.flush();
    _client.stop();
    LOGLN("<<< client disonnected");
  }
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
void WSlave::_sendHeaders(const char *codeStatus, const char *contentType)
{
  _client.print("HTTP/1.1 ");
  _client.print(codeStatus);
  _client.print(CRLF "Content-Type: ");
  _client.print(contentType);
  //_client.print(CRLF "Connection: close");
  _client.println(CRLF);
  LOG("HTTP/1.1 ");
  LOG(codeStatus);
  LOG(CRLF "Content-Type: ");
  LOG(contentType);
  //LOGLN(CRLF "Connection: close");
  LOGLN(CRLF);
}


void WSlave::_sendDictionary()
{
  //const char *strings[Core::messages_len + Core::pulses_len + Core::digitals_len];
  char pinChars[2];
  Core::_unbuffer();
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


void WSlave::_sendService()
{
  Core::_unbuffer();
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


void WSlave::_sendDefault(const prog_uchar *data, size_t length)
{
  Core::_unbuffer();
  while (length--) {
    Core::_buffer[Core::_bufferSize++] = pgm_read_byte(data++);
    Core::_autoSendBuffer();
  }
  Core::_sendBuffer();
}


/** 
  * copy the string starting here until the end character
  * into buffer (reduce the bufferSiez)
  *
  * @return false if end by a new line
  */
const boolean WSlave::_nextHttpLine()
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
