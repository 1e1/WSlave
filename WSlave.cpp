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
    _scanHttpLine(SP);
    LOG("method=");
    if (_bufferIsEqualTo("GET")) {
      LOGLN("GET");
      method = GET;
    } else if (_bufferIsEqualTo("POST")) {
      LOGLN("POST");
      method = POST;
    } else goto _send;
    
    _scanHttpLine(SP);
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
    if (method == POST) {
      LOGLN("TODO: reading body");
    }
    
    _send:
    if (method == INVALID) {
      _sendHeaders("417 Expectation failed", "text/plain");
    } else {
      switch (action) {
        case SERVICE:
        _sendHeaders("200 OK", "application/json");
        break;
        case DICTIONARY:
        _sendHeaders("200 OK", "application/json");
        _sendDictionary();
        break;
        default:
        LOG("webpage_len="); LOGLN(webpage_len);
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
  LOGLN("< send dictionnary");
  _client.write("{\"M0\":\"Hello World!\"}");
  /*
  const char *strings[messages_len()+pulses_len()+digitals_len()];
  uint8_t i = 0;
  // messages
  while (i<messages_len()) {
    i++;
  }
  // pulses
  // digitals
  // analogs
  */
}


void WSlave::_sendDefault(const prog_uchar *data, size_t length)
{
  _unbuffer();
  while (length--)
  {
    _buffer[_bufferSize++] = pgm_read_byte(data++);
  //  LOG((char) buffer[_bufferSize-1]);
    if (_bufferSize == WRITEBUFFERSIZE) {
      _client.write(_buffer, WRITEBUFFERSIZE);
      _unbuffer();
    }
  }
  if (_bufferSize) {
    _client.write(_buffer, _bufferSize);
  //  LOG(buffer[0], _bufferSize);
  }
  //LOGLN("\n |========");
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
  LOG("scanning: [");
  while (_bufferSize<READBUFFERSIZE && _client.connected() && _client.available()) {
    c = _client.read();
    LOG((char)c);
    // unprintable chars are 0x0 .. 0x1F
    // 0xE0 = 0xFF - 0x1F
    if (end!=c && (c & 0xE0)) {
      _buffer[_bufferSize++] = c;
    } else if (c & 0x1F) {
      if (previous==CR && c==LF) {
        LOGLN();
        return false;
      } else {
        previous = c;
      }
    } else break;
  }
  LOGLN(']');
  return true;
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
