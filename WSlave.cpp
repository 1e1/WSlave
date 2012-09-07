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
    boolean currentLineIsBlank = true;
    
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
    
    sendHeaders(method, action);
    
    _client.println("<!DOCTYPE HTML>");
    _client.println("<html>");
    // output the value of each analog input pin
    for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
      int sensorReading = analogRead(analogChannel);
      _client.print("analog input ");
      _client.print(analogChannel);
      _client.print(" is ");
      _client.print(sensorReading);
      _client.println("<br />");
    }
    _client.println("</html>");
    
    LOGLN("response sent");
    // give the web browser time to receive the data
    delay(1);
    _close:
    // close the connection:
    _client.flush();
    _client.stop();
    LOGLN("client disonnected");
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
  * Connection: close
  */
void WSlave::sendHeaders(const MethodType method, const ActionType action)
{
  LOGLN("HTTP response: ");
  if (method == INVALID) {
    _client.print("HTTP/1.1 417 Expectation failed");
    LOGLN(" | HTTP/1.1 417 Expectation failed");
  } else {
    _client.print("HTTP/1.1 200 OK");
    switch (action) {
      case SERVICE:
      case DICTIONARY:
      _client.println("Content-Type: application/json");
      LOGLN(" | Content-Type: application/json");
      break;
      default:
      _client.println("Content-Type: text/html");
      //_client.println("Content-Encoding: gzip");
      LOGLN(" | Content-Type: text/html");
      LOGLN(" | Content-Encoding: gzip");
    }
  }
  _client.println("Connnection: close");
  _client.println();
  LOGLN(" | Connnection: close");
  LOGLN(" |--");
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


const size_t WSlave::_bufferEqualsLength(const char *str)
{
  uint8_t i=0, j = BUFFERSIZE;
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
  _bufferSize = BUFFERSIZE;
}
