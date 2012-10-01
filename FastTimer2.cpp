#include "FastTimer2.h"




/***********************************************************
 *                       PROPERTIES                        *
 **********************************************************/




EthernetUDP FastTimer2::_server;
IPAddress FastTimer2::_timeServer(NTP_SERVER);
uint8_t FastTimer2::_embedTime = B01111111;

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


void FastTimer2::requestNtp()
{
  // http://tools.ietf.org/html/rfc1305
  byte ntp_packet[NTP_PACKET_SIZE] = {
    // LI = 11, alarm condition (clock not synchronized)
    // VN = 011, currently 3
    // VM = 011, client
    // Stratum = 0, unspecified
    // Poll Interval: 6 = 64 seconds
    // Precision: 16MHz Arduino is about 2**-24
    B11100011, 0, 6, -24,
    // Root Delay: 29s -> target less than 1 minute
    0, 29, 0, 0,
    // Root Dispersion: 29s -> target less than 1 minute
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


void FastTimer2::readNtp()
{
  unsigned long secondsSince1900, iDaySince1900;
  unsigned int dst = B0, dayOfWeek = B111, hour = B1111, iBissextile;
  byte ntp_packet[NTP_PACKET_SIZE];
  if (FastTimer2::_server.parsePacket()) {
    FastTimer2::_server.read(ntp_packet, NTP_PACKET_SIZE);
    secondsSince1900  = long(ntp_packet[40]) << 24
                      | long(ntp_packet[41]) << 16
                      | long(ntp_packet[42]) << 8
                      | long(ntp_packet[43]) << 0
                      ;
    secondsSince1900+= TZ_OFFSET *3600;
#if TZ_DST
    // dernier week-end mars <-> dernier week-end d'octobre = +3600
#endif TZ_DST
    Serial.println(secondsSince1900);
    iDaySince1900 = secondsSince1900 / 86400L;
    iBissextile   = 1 + ( (iDaySince1900 - /* first days of 1900 */ -31 -29) / 1461 );
    dayOfWeek     = iDaySince1900 %7;
    hour          = secondsSince1900/3600;
    Serial.print(dayOfWeek); Serial.println(" (day of week; 0=monday..6-sunday)");
    unsigned long secondOfDay = (secondsSince1900 % 86400L);
    Serial.print(hour%24); Serial.print(':');
    Serial.print((secondOfDay%3600)/60); Serial.print(':');
    Serial.print(secondOfDay%60); Serial.println('.');
    Serial.print(1900+((iDaySince1900-iBissextile)/365)); Serial.print("y");
    Serial.print((iDaySince1900-iBissextile)%365); Serial.print("d");
    Serial.print(""); Serial.println('.');
    Serial.print(secondOfDay); Serial.println(" (second of day)");
  }
  FastTimer2::_server.flush();
}

