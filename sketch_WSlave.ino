/*
 Rest Server
 
 https://github.com/1e1/WSlave
 
 Circuit at least one:
 * Ethernet shield
 * LCD shield
 
 created 29 Aug 2012
 by Aymeric GERLIER
 modified 4 Sep 2012
 by Aymeric GERLIER
 
 */



#include "Core2.h"
#include "config.h"
#include "macros.h"
#include "FastTimer.h"
#include "Connector.h"
#include <SPI.h>



/** ===================== **/
/**       observers       **/
/** ===================== **/

/*const*/ char obsTemperature (const byte *pins)
{
  return 'x';
}

/** ===================== **/

// Restarts program from beginning but does not reset the peripherals and registers
__attribute__((always_inline)) inline void software_reset();
void software_reset()
{
  asm volatile ("jmp 0");
}




#if USB_SPEED && !DEBUG
#include "USlave2.h"
#define USE_USB 1
#else
#define USE_USB 0
#endif

#if ETH_BLPIN && defined(IP) && defined(MAC) && defined(PORT) && defined(GATEWAY) && defined(SUBNET)
#include <Ethernet.h>
#include "WSlave2.h"
#define USE_ETH 1
#else
#define USE_ETH 0
#endif

#if LCD_BLPIN && defined(LCD_PINS) && defined(LCD_WIDTH) && defined(LCD_HEIGHT)
#include <LiquidCrystal.h>
#include "LSlave2.h"
#define USE_LCD 1
#else
#define USE_LCD 0
#endif




/** ===================== **/




void setup()
{
#if USE_USB
  USlave2::begin();
#elif DEBUG
  Serial.begin(9600);
#endif
  LOGLN();
  LOGLN("=== BEGIN SETUP ===");
#if BUSYLED_PIN
  pinMode(BUSYLED_PIN, OUTPUT);
#endif
#if USE_ETH
  // reduce DHCP timeout, default is 60000ms
  // change: Ethernet.cpp/EthernetClass::begin{...int ret = _dhcp->beginWithDHCP(mac_address);...}
  // by:     Ethernet.cpp/EthernetClass::begin{...int ret = _dhcp->beginWithDHCP(mac_address, 10000);...}
  // reduce number of client sockets to only one master (break loops)
  // change: Ethernet.h/#define MAX_SOCK_NUM 4
  // by:     Ethernet.h/#define MAX_SOCK_NUM 1
  WSlave2::begin();
  delay(1000);
#endif
#if USE_LCD
  LSlave2::begin();
  LSlave2::printInfo();
#endif
  LOGLN("=== END SETUP ===");
  LOGLN();
}


void loop()
{
  const uint8_t timer = FastTimer::update();
  if (timer) {
#if BUSYLED_PIN
  digitalWrite(BUSYLED_PIN, HIGH);
#endif
    // no true statement since half-maxlife
    if (timer > (((uint8_t)-1)>>1)) {
      LOGLN("*** new time cycle ***");
#if USE_ETH
      WSlave2::maintain();
#endif
      // OR: software_reset();
      
      /*
      // get time by NTP
      EthernetUDP Udp;
      IPAddress timeServer(85, 234, 224, 216); // 0.pool.ntp.org
      Udp.begin(8463); // TIME
      // sendNTPpacket
      delay(1000);
      if (Udp.parsePacket()) {
        //
      }
      */
      
    } else {
      LOGLN("*** new time section ***");
#if USE_LCD
      LSlave2::shutdown();
#endif
    }
    // DO SOMETHING NEW
#if BUSYLED_PIN
  WAIT(2000);
  digitalWrite(BUSYLED_PIN, LOW);
#endif
  } // if (timer)
  
  // CHECK
#if USE_ETH
  WSlave2::check();
#endif
#if USE_USB
  USlave2::check();
#endif
#if USE_LCD
  LSlave2::check();
#endif

  
  // UNCHECK
#if USE_ETH
  WSlave2::uncheck();
#endif
#if USE_USB
  USlave2::uncheck();
#endif
#if USE_LCD
  LSlave2::uncheck();
#endif
}

