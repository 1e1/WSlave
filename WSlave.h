/*
 Rest Server
 
 https://github.com/1e1/WSlave
 
 USAGE
 declaration  : static WSlave server;
 setup        : server.begin();
 loop         : server.check();
 
 URL
 GET  /       : home
 GET  /ws     : read status (no param)
 POST /D<id>  : set bool value from body to digital pin
 POST /P<id>  : set char value from body to PWM pin
 
 created 3 Sep 2012
 by Aymeric GERLIER
 
 */


#ifndef WSLAVE_H_
#define WSLAVE_H_

#include <Arduino.h>
#include <Ethernet.h>
#include <avr/pgmspace.h>
#include "Core.h"
#include "config.h"
#include "macros.h"
#include "webApp.h"


#define MAXLINESIZE 255
#define MAXHEADERS 255

// Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
#define SP ' '
#define CR '\r'
#define LF '\n'
#define CRLF "\r\n"



static byte mac[] = { MAC };
static IPAddress ip(IP);
/*
static IPAddress gateway(GATEWAY);
static IPAddress subnet(SUBNET);
*/

LONGSTRING(header_200)    = "200 OK";
LONGSTRING(header_417)    = "417 Expectation failed";
LONGSTRING(header_text)   = "text/plain";
LONGSTRING(header_json)   = "application/json";
LONGSTRING(header_htZ)    = "text/html" CRLF "Content-Encoding: gzip";
LONGBYTES(webpage)        = WEBPAGE;
static size_t webpage_len = ARRAYLEN(webpage); // ~ 1557o / 1600o / 1709o / 2100o



namespace WSlave {
  
  void begin();
  void check();
  void maintain();
  
  // http://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html
  enum MethodType     { INVALID/*, OPTIONS*/, GET/*, HEAD, POST*/, PUT/*, DELETE, TRACE, CONNECT*/ };
  enum ActionType     { ROOT, SERVICE, CACHE, DICTIONARY };
  
  static EthernetServer _server(PORT);
  static EthernetClient _client;
  
  inline void _sendHeaders_P(const prog_uchar *codeStatus, const prog_uchar *contentType);
  void _sendDictionary();
  void _sendService();
  void _sendDefault_P(const prog_uchar data[], size_t length);
  const boolean _nextHttpLine();
  
};


#endif WSLAVE_H_

