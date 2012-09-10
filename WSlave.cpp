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
    _unbuffer();
    _client.readBytesUntil(SP, _buffer, READBUFFERSIZE);
    LOG("method=");
    if (_bufferIsEqualTo("GET")) {
      LOGLN("GET");
      method = GET;
    } else if (_bufferIsEqualTo("PUT")) {
      LOGLN("PUT");
      method = PUT;
    } else goto _send;
    
    _unbuffer();
    _client.readBytesUntil(SP, _buffer, READBUFFERSIZE);
    LOG("action=");
    if (_bufferIsPrefixOf("/ws")) {
      action = SERVICE;
      LOGLN("webservice");
      // TODO catch /ws?param!!!
    } else if (_bufferIsPrefixOf("/dict")) {
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
    if (_nextHttpLine() && watchdog) goto _crlfcrlf;
    if (!watchdog) {
      method = INVALID;
    }
    //LOGLN("ready to read body");
    
    // on body:
    if (method == PUT && action == SERVICE) {
      LOGLN("TODO: reading body");
      // (D|P)[0-9]+=[0-9]+
      _processOneParameter();
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
  _unbuffer();
  _copyToBuffer('{');
  // messages
  for (uint8_t i=0; i < Core::messages_len; i++) {
    Core::pinToChars(i, pinChars);
    _copyJsonToBuffer('M', pinChars, Core::messages[i].label);
  }
  // pulses
  for (uint8_t i=0; i < Core::pulses_len; i++) {
    Core::pinToChars(Core::pulses[i].pin, pinChars);
    _copyJsonToBuffer('P', pinChars, Core::pulses[i].label);
  }
  // digitals
  for (uint8_t i=0; i < Core::digitals_len; i++) {
    Core::pinToChars(Core::digitals[i].vPin, pinChars);
    _copyJsonToBuffer('D', pinChars, Core::digitals[i].label);
  }
  _copyToBuffer("\"M#\":\"FastTimer\"}");
  _sendBuffer();
}


void WSlave::_sendService()
{
  _unbuffer();
  _copyToBuffer('[');
  // messages
  for (uint8_t i=0; i < Core::messages_len; i++) {
    _copyToBuffer(Core::messages[i].value);
    _copyToBuffer(',');
  }
  // pulses
  for (uint8_t i=0; i < Core::pulses_len; i++) {
    _copyToBuffer(PULSE_VALUE_AT(i));
    _copyToBuffer(',');
  }
  // digitals
  for (uint8_t i=0; i < Core::digitals_len; i++) {
    _copyToBuffer((uint8_t)DIGITAL_VALUE_AT(i));
    _copyToBuffer(',');
  }
  _copyToBuffer("\"#\"");
  _copyToBuffer(']');
  _sendBuffer();
}


void WSlave::_sendDefault(const prog_uchar *data, size_t length)
{
  _unbuffer();
  while (length--) {
    _buffer[_bufferSize++] = pgm_read_byte(data++);
    _autoSendBuffer();
  }
  _sendBuffer();
}


void WSlave::_copyToBuffer(uint8_t x)
{
  do {
    _copyToBuffer((char) ('0'+(x%10)));
    x/= 10;
  } while (x);
}


void WSlave::_copyToBuffer(const char c)
{
  _buffer[_bufferSize++] = c;
  _autoSendBuffer();
}


void WSlave::_copyToBuffer(const char* str)
{
  while (*str) {
    _buffer[_bufferSize++] = *str++;
    _autoSendBuffer();
  }
}


void WSlave::_copyToBuffer(const char chars[], uint8_t size)
{
  for (uint8_t i=0; i<size; i++) {
    _buffer[_bufferSize++] = chars[i];
    _autoSendBuffer();
  }
}


void WSlave::_copyJsonToBuffer(const char type, const char *pinChars, const char *label)
{
  _copyToBuffer('"');
  _copyToBuffer(type);
  _copyToBuffer(pinChars, 2);
  _copyToBuffer("\":\"");
  _copyToBuffer(label);
  _copyToBuffer("\",");
}


void WSlave::_autoSendBuffer()
{
  if (_bufferSize == WRITEBUFFERSIZE) {
    _client.write((uint8_t *)_buffer, WRITEBUFFERSIZE);
    _unbuffer();
  }
}


void WSlave::_sendBuffer()
{
  if (_bufferSize) {
    _client.write((uint8_t *)_buffer, _bufferSize);
  }
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


/** 
  * body: (D|P)[0-9]+=[0-9]+)
  * @return boolean true if last char is '&'
  */
const boolean WSlave::_processOneParameter()
{
  ParameterType type = UNKNOWN;
  uint8_t pin = 0;
  uint8_t value = 0;
  int c;
  if (_client.connected() && _client.available()) {
     // read 1st char
     c = _client.read();
     switch (c) {
       case 'D':
       type = DIGITAL;
       LOG("SET DIGITAL #");
       break;
       case 'P':
       type = PULSE;
       LOG("SET PULSE #");
       break;
       default:
       goto _end;
     }
     _readUint8(pin);
     _readUint8(value);
     LOG(pin);
     LOG(" = ");
     LOG(value);
     LOGLN(';');
     switch (c) {
       case 'D':
       Core::setDigitalAtPin(pin, value);
       break;
       case 'P':
       Core::setPulseAtPin(pin, value);
       break;
     }
  }
  _end:
  return false;
}


void WSlave::_readUint8(uint8_t &out)
{
  int c;
  while (_client.connected()/* && _client.available()*/ && (c=_client.read()) && '0'<=c && 'c'<='9') {
    out = (out *10) + ((uint8_t) (c -'0'));
  }
}


const uint8_t WSlave::_bufferEqualsLength(const char *str)
{
  uint8_t i=0;
  while (i<_bufferSize && str[i]==_buffer[i]) i++;
  return i;
}


const boolean WSlave::_bufferIsEqualTo(const char *str)
{
  return _bufferSize == strlen(str) && _bufferEqualsLength(str) == strlen(str);
}


const uint8_t WSlave::_bufferIsPrefixOf(const char *str)
{
  return _bufferEqualsLength(str) == strlen(str);
}


void WSlave::_unbuffer()
{
  _bufferSize = 0;
}
