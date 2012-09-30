#include "FastTimer2.h"




/***********************************************************
 *                       PROPERTIES                        *
 **********************************************************/




EthernetUDP FastTimer2::_server;
IPAddress FastTimer2::_timeServer(NTP_SERVER);
uint8_t FastTimer2::_embedTime = 0;

// http://tools.ietf.org/html/rfc1305
LONGBYTEN(ntp_packet, NTP_PACKET_SIZE) = {
  // LI = 11, alarm condition (clock not synchronized)
  // VN = 011, currently 3
  // VM = 011, client
  // Stratum = 0, unspecified
  // Poll Interval: 6 = 64 seconds
  // Precision: 16MHz Arduino is 2**-24
  B11100011, 0, 6, -24,
  // Root Delay
  0, 29, 0, 0,
  // Root Dispersion
  0, 29, 0, 0,
  // Reference Clock Identifier
  IP //0, 0, 0, DEVICE_NUMBER,
  //0
};




/***********************************************************
 *                         PUBLIC                          *
 **********************************************************/




void FastTimer2::begin()
{
  _server.begin(NTP_LOCALPORT);
}


/** 
  *   0: same time section
  *   1: new time section
  * 255: new time section, new cycle
  * 
  * @return int8_t
  */
const uint8_t FastTimer2::update()
{
  const uint8_t previousTime = FastTimer2::_embedTime;
  FastTimer2::_embedTime = EMBEDTIME;
  return FastTimer2::_embedTime ^ previousTime;
}


const void FastTimer2::requestNtp()
{
  // http://tools.ietf.org/html/rfc1305
  byte ntp_packet[NTP_PACKET_SIZE] = {
    // LI = 11, alarm condition (clock not synchronized)
    // VN = 011, currently 3
    // VM = 011, client
    // Stratum = 0, unspecified
    // Poll Interval: 6 = 64 seconds
    // Precision: 16MHz Arduino is 2**-24
    B11100011, 0, 6, -24,
    // Root Delay
    0, 29, 0, 0,
    // Root Dispersion
    0, 29, 0, 0,
    // Reference Clock Identifier
    IP //0, 0, 0, DEVICE_NUMBER,
    //0
  };
  FastTimer2::_server.beginPacket(FastTimer2::_timeServer, NTP_PORT);
  /*
  for (uint8_t i=0; i<NTP_PACKET_SIZE; i++) {
    server.write(pgm_read_byte_near(&ntp_packet[i]));
  }
  */
  FastTimer2::_server.write(ntp_packet, NTP_PACKET_SIZE);
  FastTimer2::_server.endPacket();
}


const unsigned long FastTimer2::readNtp()
{
  unsigned long time = 0;
  uint8_t dayOfWeek = 7;
  byte ntp_packet[NTP_PACKET_SIZE];
  if (FastTimer2::_server.parsePacket()) {
    FastTimer2::_server.read(ntp_packet, NTP_PACKET_SIZE);
    time  = long(ntp_packet[40]) << 24
          | long(ntp_packet[41]) << 16
          | long(ntp_packet[42]) << 8
          | long(ntp_packet[43]) << 0
          ;
    time+= TZ_OFFSET *3600;
    Serial.println((unsigned long)time);
    dayOfWeek = (time / 86400L) %7;
    Serial.println(dayOfWeek);
    unsigned long secondOfDay = (time % 86400L);
    Serial.print(secondOfDay/3600); Serial.print(':');
    Serial.print((secondOfDay%3600)/60); Serial.print(':');
    Serial.print(secondOfDay%60); Serial.println('.');
    Serial.println(secondOfDay);
#if TZ_DAYLIGHT
    // dernier week-end mars <-> dernier week-end d'octobre = +3600
#endif TZ_DAYLIGHT
  }
  FastTimer2::_server.flush();
  return time;
}

