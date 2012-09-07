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
#include "config.h"
#include "macros.h"


#define BUFFERSIZE 8
#define MAXLINESIZE 255
#define MAXHEADERS 255
#define RESETBUFFER (_bufferSize = BUFFERSIZE)

// Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
#define SP ' '
#define CR '\r'
#define LF '\n'
#define CRLF "\r\n"



// 3 bytes = byte + @char
// readOnly
struct intfDigital
{
  // mega has 0..69 pin
  // byte [0..255]
  // digital values: 0..1
  // first byte is the value
  // bitRead(value, bit)
  // bitSet(value, bit)
  // bitClear(value, bit)
  // bitWrite(value, bit, bitvalue)
  byte vPin;
  const char *label;
};
// { 13+0b10000000 , 0, "Light" }
// { 13+128 , 0, "Light" }
// { 13+0x80 , 0, "Light" }

// 4 bytes = byte + uint8_t + @char
// readOrWrite (but not both)
struct intfPulse
{
  // mega has 0..69 pin
  // byte [0..255]
  // analog Read values: 0..1023 -> translate value>>8
  // analog Write values: 0..255
  // first byte is writeAccess
  // digitalPinHasPWM(p)
  byte wPin;
  uint8_t value;
  const char *label;
};
// { 13, 0, "Light" }

// 4 bytes = byte + uint8_t + @char
// readOrWrite (but not both)
struct intfMessage
{
  // mega has 0..69 pin
  // char [-128..127]
  char (*observe) (byte *pins);
  char value;
  const byte *pins;
  const char *label;
};
// test[i].value = test[i].observe(test[i].pins)
// { obsTemperature, "##", { 13 }, "Outdoor" }



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
    inline void sendHeaders(const MethodType method, const ActionType action);
    const boolean _nextHttpLine();
    const boolean _scanHttpLine(const char end);
    const size_t _bufferEqualsLength(const char *str);
    inline const boolean _bufferIsEqualTo(const char *str);
    __attribute__((always_inline)) inline const boolean _bufferIsPrefixOf(const char *str);
    __attribute__((always_inline)) inline void _unbuffer();
    
    char _reverseBuffer[BUFFERSIZE];
    size_t _bufferSize;
  
};


#endif WSLAVE_H_

