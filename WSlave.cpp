#include "WSlave.h"



WSlave::WSlave() :
  _server(PORT)/*,
  _client()*/
{
  // RESETBUFFER //_unbuffer();
}


void WSlave::begin()
{
  _server.begin();
}


void WSlave::check()
{
  if (_client = _server.available()) {
    LOGLN("new client");
    
    MethodType method = INVALID;
    ActionType action = ROOT;
    uint8_t watchdog  = MAXHEADERS;
    
    // Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
    _scanHttpLine(SP);
    if (_bufferIsEqualTo("GET")) {
      LOGLN("GET");
      method = GET;
    } else if (_bufferIsEqualTo("POST")) {
      LOGLN("POST");
      method = POST;
    } else goto _close;
    
    _scanHttpLine(SP);
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
    LOGLN("sweeping headers");
    _crlfcrlf:
    while (_nextHttpLine() && --watchdog);
    if (_nextHttpLine() && watchdog) goto _crlfcrlf;
    if (!watchdog) {
      method = INVALID;
    }
    LOGLN("ready to read body");
    
    // on body:
    if (method == POST) {
      LOGLN("reading body");
    }
    
    _sendHeaders(method, action);
    //_client.println("<!DOCTYPE HTML>");
    //_client.println("<html><body><h1>OK</h1></body></html>");
    
    //if (method!=HEAD) {
      switch (action) {
        
        default:
        _sendBody(webpage, webpage_len);
      }
    //} // if (method!=HEAD)
    
    LOGLN("response sent");
    // give the web browser time to receive the data
    delay(1);
    _close:
    // close the connection:
    //_client.flush();
    _client.stop();
    LOGLN("client disonnected");
  }
}


void WSlave::_setDictionary()
{
/*
  const string strings[ARRAYLEN(messages)+ARRAYLEN(pulses)+ARRAYLEN(digitals)];
  uint8_t i;
  // messages
  for (i=0; i<ARRAYLEN(messages); i++) {
    
  }
  // pulses
  // digitals
  // analogs
*/
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
void WSlave::_sendHeaders(const MethodType method, const ActionType action)
{
  LOGLN("HTTP response: ");
  if (method == INVALID) {
    _client.println("HTTP/1.1 417 Expectation failed");
    LOGLN(" | HTTP/1.1 417 Expectation failed");
  } else {
    _client.println("HTTP/1.1 200 OK");
    LOGLN(" | HTTP/1.1 200 OK");
    switch (action) {
      case SERVICE:
      case DICTIONARY:
      _client.println("Content-Type: application/json"); // ; charset=utf-8
      LOGLN(" | Content-Type: application/json"); // ; charset=utf-8
      break;
      default:
      _client.println("Content-Type: text/html"); // ; charset=utf-8
      _client.println("Content-Encoding: gzip"); // ; charset=utf-8
      LOGLN(" | Content-Type: text/html");
      LOGLN(" | Content-Encoding: gzip");
    }
  }
  //_client.println("Connection: close");
  _client.println();
  LOGLN(" | Connection: close");
  LOGLN(" |--------");
}


void WSlave::_sendBody(const prog_uchar *bytes)
{
  uint8_t buffer[WRITEBUFFERSIZE];
  uint8_t i = 0;
  while ((buffer[i++] = pgm_read_byte(bytes++)))
  {
    LOG((char) buffer[i-1]);
    if (i == WRITEBUFFERSIZE) {
      _client.write(buffer, WRITEBUFFERSIZE);
      i = 0;
    }
  }
  if (i) {
    _client.write(buffer, i);
    LOG(buffer[0], i);
  }
  LOGLN("\n |========");
}


void WSlave::_sendBody(const prog_uchar *data, size_t length)
{
  uint8_t buffer[WRITEBUFFERSIZE];
  uint8_t i = 0;
  while (length--)
  {
    if (i == WRITEBUFFERSIZE) {
      _client.write(buffer, WRITEBUFFERSIZE);
      i = 0;
    }
    buffer[i++] = pgm_read_byte(data++);
    LOG((char) buffer[i-1]);
  }
  if (i) {
    _client.write(buffer, i);
    LOG(buffer[0], i);
  }
  LOGLN("\n |========");
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
  * copy the string starting here until the end character
  * into buffer (reduce the bufferSiez)
  * 
  * @param end   searched char
  * @param flags <allow multilines> <fail at end>
  * @return false if end by a new line
  */
const boolean WSlave::_scanHttpLine(const char end)
{
  _unbuffer();
  int c;
  char previous = '\0';
  while (_bufferSize && _client.connected() && _client.available()) {
    c = _client.read();
    // unprintable chars are 0x0 .. 0x1F
    // 0xE0 = 0xFF - 0x1F
    if (end!=c && (c & 0xE0)) {
      _reverseBuffer[--_bufferSize] = c;
    } else if (c & 0x1F) {
      if (previous==CR && c==LF) {
        return false;
      } else {
        previous = c;
      }
    } else break;
  }
  return true;
}


const uint8_t WSlave::_bufferEqualsLength(const char *str)
{
  uint8_t i=0, j = READBUFFERSIZE;
  while(_bufferSize<j && str[i]==_reverseBuffer[--j]) {
    i++;
  }
  return i;
}


const boolean WSlave::_bufferIsEqualTo(const char *str)
{
  return _bufferEqualsLength(str)==strlen(str);
}


const boolean WSlave::_bufferIsPrefixOf(const char *str)
{
  return _bufferSize == strlen(str) && _bufferEqualsLength(str)==strlen(str);
}


void WSlave::_unbuffer()
{
  _bufferSize = READBUFFERSIZE;
}
