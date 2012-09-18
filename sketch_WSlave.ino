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



#include "Core.h"
#include "config.h"
#include "macros.h"
#include "dictionary.h"
#include "FastTimer.h"
#include "ConnectorDigital.h"
#include "ConnectorPulse.h"
#include <SPI.h>



/** ===================== **/
/**      dictionary       **/
/** ===================== **/

namespace Dictionary {
  LONGSTRING(relay11) = "relay1.1";
  LONGSTRING(relay12) = "relay1.2";
  LONGSTRING(relay13) = "relay1.3";
  LONGSTRING(relay14) = "relay1.4";
  LONGSTRING(led) = "pulse";
}


/** ===================== **/
/**      connections      **/
/** ===================== **/

static const ConnectorDigital digitals[] = {
  ConnectorDigital(22, Dictionary::relay11, true) ,
  ConnectorDigital(24, Dictionary::relay12, false),
  ConnectorDigital(26, Dictionary::relay13, false),
  ConnectorDigital(28, Dictionary::relay14, false)
};

static const ConnectorPulse pulses[] = {
  ConnectorPulse(13, Dictionary::led)
};

// DRAFT
static const byte temperaturePins[] = { 13 };
const intfMessage messages[] = {
  { obsTemperature, 'x', temperaturePins, "Indoor" }
};

static const uint8_t digitals_len = ARRAYLEN(digitals);
static const uint8_t pulses_len   = ARRAYLEN(pulses);
static const uint8_t messages_len = ARRAYLEN(messages);
static const uint8_t total_len    = ARRAYLEN(digitals) + ARRAYLEN(pulses) + ARRAYLEN(messages);

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




#if defined(USB_SPEED) //&& !DEBUG
#include "USlave.h"
#define USE_USB 1
#else
#define USE_USB 0
#endif

#if defined(IP) && defined(MAC) && defined(PORT) && defined(GATEWAY) && defined(SUBNET) && defined(ETH_BLPIN)
#include <Ethernet.h>
#include "WSlave.h"
#define USE_ETH 1
#else
#define USE_ETH 0
#endif

#if defined(LCD_PINS) && defined(LCD_WIDTH) && defined(LCD_HEIGHT) && defined(LCD_BLPIN)
#include <LiquidCrystal.h>
#include "LSlave.h"
#define USE_LCD 1
#else
#define USE_LCD 0
#endif




/** ===================== **/




void setup()
{
#if USE_USB
  USlave::begin();
#elif DEBUG
  Serial.begin(9600);
#endif
  LOGLN();
  LOGLN("=== BEGIN SETUP ===");
#if BUSYLED_PIN
  pinMode(BUSYLED_PIN, OUTPUT);
#endif
  Core::setup();
#if USE_ETH
  // reduce DHCP timeout, default is 60000ms
  // change: Ethernet.cpp/EthernetClass::begin{...int ret = _dhcp->beginWithDHCP(mac_address);...}
  // by:     Ethernet.cpp/EthernetClass::begin{...int ret = _dhcp->beginWithDHCP(mac_address, 10000);...}
  // reduce number of client sockets to only one master (break loops)
  // change: Ethernet.h/#define MAX_SOCK_NUM 4
  // by:     Ethernet.h/#define MAX_SOCK_NUM 1
  WSlave::begin();
  delay(1000);
#endif
#if USE_LCD
  LSlave::begin();
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
      WSlave::maintain();
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
      LSlave::shutdown();
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
  WSlave::check();
#endif
#if USE_USB
  USlave::check();
#endif
#if USE_LCD
  LSlave::check();
#endif

  
  // UNCHECK
#if USE_ETH
  WSlave::uncheck();
#endif
#if USE_USB
  USlave::uncheck();
#endif
#if USE_LCD
  LSlave::uncheck();
#endif
}

