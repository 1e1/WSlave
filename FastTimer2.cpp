#include "FastTimer2.h"




/***********************************************************
 *                       PROPERTIES                        *
 **********************************************************/




EthernetUDP FastTimer2::_server;
IPAddress FastTimer2::_timeServer(NTP_SERVER);
uint8_t FastTimer2::_embedTime      = 0;
uint8_t FastTimer2::_referenceTime  = B01111111;




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
  unsigned int dst = B0, dayOfWeek = B111, hour = B1111;
  int dayOfYear;
  uint8_t yearSince1900, iLeapYear, deltaDays;
  byte ntp_packet[NTP_PACKET_SIZE];
  if (FastTimer2::_server.parsePacket()) {
    FastTimer2::_referenceTime = 0;
    FastTimer2::_server.read(ntp_packet, NTP_PACKET_SIZE);
    secondsSince1900  = long(ntp_packet[40]) << 24
                      | long(ntp_packet[41]) << 16
                      | long(ntp_packet[42]) << 8
                      | long(ntp_packet[43]) << 0
                      ;
    secondsSince1900+= TZ_OFFSET *3600;
    LOGLN(secondsSince1900);
    iDaySince1900 = secondsSince1900 / 86400L;
    iLeapYear     = ( (iDaySince1900 /* first days of 1900 */ -31 -28) / (365*4 +1) );
    yearSince1900 = ( iDaySince1900 - iLeapYear ) / 365;
    dayOfYear     = 1 + ((iDaySince1900 - iLeapYear) % 365);
    dayOfWeek     = (MONDAY + iDaySince1900) % 7;
    hour          = (secondsSince1900 / 3600) % 24;
#if TZ_DST
    // http://www.legifrance.gouv.fr/affichTexte.do?cidTexte=JORFTEXT000000221946&dateTexte=&categorieLien=id
    //( ? + year + int(year/4) /* Arduino will die before ( */ - int(year/100) + int(year/400) /* ) */ ) % 7;
    // work until 2104 = 1900 + 204
    // 255 = (204 + 204/4)
    deltaDays = yearSince1900 + iLeapYear;
    dayOfYear-= (31 + 28 + 31);
    if ((yearSince1900 % 4) == 0) {
      dayOfYear--;
    }
    // last sunday of march = 31 - ( (SATURDAY + deltaDays) % 7 );
    if (dayOfYear > ((SATURDAY + deltaDays) % 7)) {
      
      dayOfYear-= (30 + 31 + 30 + 31 + 31 + 30 + 31);
      // last sunday of october = 31 - ( (WEDNESDAY  + deltaDays) % 7 );
      if (dayOfYear < ((WEDNESDAY + deltaDays) % 7)) {
        
        // DST days
        dst = B1;
        hour++;
        
      } else if (dayOfYear == ((SATURDAY + deltaDays) % 7)) {
        
        // last DST day
        if (hour<2) {
          dst = B1;
          hour++;
        }
        
      }
      
    } else if (dayOfYear == ((WEDNESDAY + deltaDays) % 7)) {
      
      // first DST day
      if (hour >= 2) {
        dst = B1;
        hour++;
      }
      
    }
    
    if (hour==24) {
      dayOfYear++;
      secondsSince1900+= 3600;
      dayOfWeek = (dayOfWeek + 1) %7;
    }
#endif TZ_DST
    FastTimer2::_referenceTime = (dst << 7) | (dayOfWeek << 4) | (hour);
    
    unsigned long secondOfDay = (secondsSince1900 % 86400L);
    unsigned int year = 1900 + ( (iDaySince1900-iLeapYear) / 365 );
    
    LOG(dayOfWeek);   LOGLN(" (day of week; 0=monday..6=sunday)");
    LOG(hour%24);     LOG(':');     LOG((secondOfDay%3600)/60);   LOG(':'); LOG(secondOfDay%60);  LOGLN('.');
    LOG(year);        LOG("y ");    LOG(dayOfYear);               LOGLN("d");
    LOG(secondOfDay); LOGLN(" (second of day)");
  }
  FastTimer2::_server.flush();
}

