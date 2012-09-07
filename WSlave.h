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
#include "core.h"
#include "config.h"
#include "macros.h"
#include "webApp.h"


#define READBUFFERSIZE 8
#define WRITEBUFFERSIZE 32
#define MAXLINESIZE 255
#define MAXHEADERS 255
#define RESETBUFFER (_bufferSize = READBUFFERSIZE)

// Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
#define SP ' '
#define CR '\r'
#define LF '\n'
#define CRLF "\r\n"



LONGBYTES(webpage) = WEBPAGE;



class WSlave {
  
  public:
    WSlave();
    
    void begin();
    void check();
  
  private:
    // http://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html
    enum MethodType { INVALID/*, OPTIONS*/, GET/*, HEAD*/, POST/*, PUT, DELETE, TRACE, CONNECT*/ };
    enum ActionType { ROOT, SERVICE, CACHE, DICTIONARY };
    
    EthernetServer _server;
    EthernetClient _client;
    void _setDictionary();
    inline void _sendHeaders(const MethodType method, const ActionType action);
    void _sendBody(const prog_uchar bytes[]);
    const boolean _nextHttpLine();
    const boolean _scanHttpLine(const char end);
    const size_t _bufferEqualsLength(const char *str);
    inline const boolean _bufferIsEqualTo(const char *str);
    __attribute__((always_inline)) inline const boolean _bufferIsPrefixOf(const char *str);
    __attribute__((always_inline)) inline void _unbuffer();
    
    char _reverseBuffer[READBUFFERSIZE];
    size_t _bufferSize;
  
};


#endif WSLAVE_H_

