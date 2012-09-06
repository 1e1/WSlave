#include "WSlave.h"

#include "config.h"
#include "macros.h"


WSlave::WSlave() :
  _server(PORT)
{
  _unbuffer();
}

void WSlave::begin()
{
  _server.begin();
}

void WSlave::check()
{
  if (_client = _server.available()) {
    // _readMethod()
    // _readPath()
    // _readHeaders()
    // switch strcmp("*", path)
    
    LOGLN("new client");
    
    // Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
    MethodType method = INVALID;
    _scanHttpLine(SP);
    if (_bufferIsEqualTo("GET")) {
      LOGLN("GET");
      method = GET;
    } else if (_bufferIsEqualTo("POST")) {
      LOGLN("POST");
      method = POST;
    }
    _scanHttpLine(SP);
    if (_bufferEqualsLength("/ws")==3) {
      LOGLN("webservice");
      // TODO catch /ws?param!!!
    } else if (_bufferEqualsLength("/dict")==5) {
      LOGLN("dictionary");
    } else {
      LOGLN("*");
    }
    _nextHttpLine();
    
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (_client.connected()) {
      if (_client.available()) {
        char c = _client.read();
        LOG(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (currentLineIsBlank && '\n' == c) {
          // send a standard http response header
          _client.println("HTTP/1.1 200 OK");
          _client.println("Content-Type: text/html");
          _client.println("Connnection: close");
          _client.println();
          _client.println("<!DOCTYPE HTML>");
          _client.println("<html>");
                    // add a meta refresh tag, so the browser pulls again every 5 seconds:
          _client.println("<meta http-equiv=\"refresh\" content=\"5\">");
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
          break;
        }
        if ('\n' == c) {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if ('\r' != c) {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    _client.flush();
    _client.stop();
    LOGLN("client disonnected");
  }
}


/** 
  * copy the string starting here until the end character
  * into buffer (reduce the bufferSiez)
  * 
  * @param end   searched char
  * @param flags <allow multilines> <fail at end>
  */
void WSlave::_nextHttpLine()
{
  int c;
  uint8_t watchdog = BUFFERSIZE;
  carriageReturn:
  while (watchdog-- && _client.connected() && _client.available() && _client.read() != CR);
  lineFeed:
  if (watchdog-- && _client.connected() && _client.available() && _client.read() != LF) {
    goto carriageReturn;
  }
}


/** 
  * copy the string starting here until the end character
  * into buffer (reduce the bufferSiez)
  * 
  * @param end   searched char
  * @param flags <allow multilines> <fail at end>
  */
void WSlave::_scanHttpLine(const char end)
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
        return;
      } else {
        previous = c;
      }
    } else return;
  }
}


const uint8_t WSlave::_bufferEqualsLength(const char *str)
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
