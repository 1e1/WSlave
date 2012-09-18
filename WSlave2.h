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


#ifndef WSLAVE2_H_
#define WSLAVE2_H_

#include <Arduino.h>
#include <Ethernet.h>
#include <avr/pgmspace.h>
#include "Core2.h"
#include "config.h"
#include "macros.h"
#include "webApp.h"


#define MAXHEADERS 255

// Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
#define SP ' '
#define CR '\r'
#define LF '\n'
#define CRLF "\r\n"



class WSlave2 {
  
  public:
  static void begin();
  static void check();
  static void uncheck();
  static void maintain();
  
  // http://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html
  typedef enum { INVALID/*, OPTIONS*/, GET/*, HEAD, POST*/, PUT/*, DELETE, TRACE, CONNECT*/ } MethodType;
  typedef enum { ROOT, SERVICE, CACHE, DICTIONARY } ActionType;
  
  protected:
  static EthernetServer _server;
  static EthernetClient _client;
  
  static void sendHeaders_P(const prog_char *codeStatus, const prog_char *contentType);
  static void sendDictionary();
  static void sendService();
  static void sendDefault_P(const prog_uchar data[], size_t length);
  static const boolean nextHttpLine();
  static const uint8_t lineLength();
  static void sendToJson(const char type, Connector connector, const uint8_t coma);
  /*
  LONGSTRING(header_200)    = "200 OK";
  LONGSTRING(header_417)    = "417 Expectation failed";
  LONGSTRING(header_text)   = "text/plain" CRLF;
  LONGSTRING(header_json)   = "application/json" CRLF;
  LONGSTRING(header_htZ)    = "text/html" CRLF "Content-Encoding: gzip";
  LONGBYTES(webpage)        = WEBPAGE;
  static size_t webpage_len = ARRAYLEN(webpage); // ~ 1557o / 1600o / 1709o / 2100o
  */
};




/***********************************************************
 *                         INLINE                          *
 **********************************************************/


__attribute__((always_inline)) inline void WSlave2::uncheck()
{
  //_client.flush();
  _client.stop();
}


__attribute__((always_inline)) inline void WSlave2::maintain()
{
#if USE_DHCP
  Ethernet.maintain(); /* added in 1.0.1 - default Ubuntu IDE is still in 1.0 */
  LOGLN("renew DHCP");
#endif
}


#endif WSLAVE_H2_
