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
    _scan(SP);
    if (_bufferEquals("GET")) {
      LOGLN("GET");
      method = GET;
    } else if (_bufferEquals("POST")) {
      LOGLN("POST");
      method = POST;
    }
    _scan(SP);
    if (_bufferEquals("/ws")) {
      LOGLN("webservice");
    } else {
      // TODO catch /ws?param!!!
      LOGLN("*");
    }
    
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
void WSlave::_scan(const char end)
{
  _unbuffer();
  int c;
  while (_bufferSize && _client.connected() && _client.available()) {
    c = _client.read();
    if (end!=c && 0<c) {
      _reverseBuffer[--_bufferSize] = c;
    } else break;
  }
}


boolean WSlave::_bufferEquals(const char *str)
{
  if ('\0' != str[_bufferSize]) {
    return false;
  }
  uint8_t i=0, j=_bufferSize;
  while (i<j--) {
    if (str[i]!=_reverseBuffer[j] || str[j]!=_reverseBuffer[i]) {
      return false;
    }
    i++;
  }
  return true;
}


void WSlave::_unbuffer()
{
  _bufferSize = BUFFERSIZE;
}
