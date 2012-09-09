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


#define READBUFFERSIZE 8
#define WRITEBUFFERSIZE 64
#define MAXLINESIZE 255
#define MAXHEADERS 255

// Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
#define SP ' '
#define CR '\r'
#define LF '\n'
#define CRLF "\r\n"



LONGBYTES(webpage) = WEBPAGE;
static size_t webpage_len = ARRAYLEN(webpage); // ~ 1709o



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
    inline void _sendHeaders(const char *codeStatus, const char *contentType);
    void _sendDictionary();
    void _sendDefault(const prog_uchar data[], size_t length);
    inline void _copyToBuffer(char c);
    void _copyToBuffer(const char* str);
    void _copyToBuffer(const char chars[], uint8_t size);
    inline void _autoSendBuffer();
    inline void _sendBuffer();
    const boolean _nextHttpLine();
    const boolean _scanHttpLine(const char end);
    const uint8_t _bufferEqualsLength(const char *str);
    __attribute__((always_inline)) inline const boolean _bufferIsEqualTo(const char *str);
    __attribute__((always_inline)) inline const uint8_t _bufferIsPrefixOf(const char *str);
    __attribute__((always_inline)) inline void _unbuffer();
    
    uint8_t _buffer[max(READBUFFERSIZE, WRITEBUFFERSIZE)];
    uint8_t _bufferSize;
  
};


#endif WSLAVE_H_

