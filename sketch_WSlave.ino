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
#include "FastTimer.h"
#include <SPI.h>



/** ===================== **/
/**      connections      **/
/** ===================== **/

const intfDigital digitals[] = {
  WSDIGITAL("relay1.1", 22),
  WSDIGITAL("relay1.2", 24),
  WSDIGITAL("relay1.3", 26),
  WSDIGITAL("relay1.4", 28),
  WSDIGITAL("relay1.5", 30),
  WSDIGITAL("relay1.6", 32),
  WSDIGITAL("relay1.7", 34),
  WSDIGITAL("relay1.8", 36)
};
const intfPulse pulses[] = {
  WSPULSE("Pulse", 24)
};
static const byte temperaturePins[] = { 13 };
const intfMessage messages[] = {
  { obsTemperature, 'x', temperaturePins, "Indoor" }
};

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

__attribute__((always_inline)) inline const uint8_t digitals_len();
__attribute__((always_inline)) inline const uint8_t pulses_len();
__attribute__((always_inline)) inline const uint8_t messages_len();
const uint8_t digitals_len() { return ARRAYLEN(digitals); }
const uint8_t pulses_len()   { return ARRAYLEN(pulses);   }
const uint8_t messages_len() { return ARRAYLEN(messages); }




#if defined(LCD_PINS) && defined(LCD_WIDTH) && defined(LCD_HEIGHT) && defined(LCD_BLPIN)
#include <LiquidCrystal.h>
#include "LSlave.h"
static LiquidCrystal lcd(LCD_PINS);
static char lcdLines[LCD_HEIGHT][LCD_WIDTH];
#define USE_LCD 1
#else
#define USE_LCD 0
#endif

#if defined(IP) && defined(MAC) && defined(PORT) && defined(GATEWAY) && defined(SUBNET)
#include <Ethernet.h>
#include "WSlave.h"
static byte mac[] = { MAC };
static IPAddress ip(IP);
/*
static IPAddress gateway(GATEWAY);
static IPAddress subnet(SUBNET);
*/
static WSlave wsengine;
#define USE_ETH 1
#else
#define USE_ETH 0
#endif




/** ===================== **/




void setup()
{
#if DEBUG
  Serial.begin(9600);
#endif
  LOGLN();
  LOGLN("=== BEGIN SETUP ===");
#if BUSYLED_PIN
  pinMode(BUSYLED_PIN, OUTPUT);
#endif
#if USE_LCD
  lcd.begin(LCD_WIDTH, LCD_HEIGHT);
  pinMode(LCD_BLPIN, OUTPUT);
  digitalWrite(LCD_BLPIN, HIGH);
#endif
#if USE_ETH
  LOGLN("Trying to get an IP address using DHCP");
  // reduce DHCP timeout, default is 60000ms
  // change: Ethernet.cpp/EthernetClass::begin{...int ret = _dhcp->beginWithDHCP(mac_address);...}
  // by:     Ethernet.cpp/EthernetClass::begin{...int ret = _dhcp->beginWithDHCP(mac_address, 10000);...}
  // reduce number of client sockets to only one master (break loops)
  // change: Ethernet.h/#define MAX_SOCK_NUM 4
  // by:     Ethernet.h/#define MAX_SOCK_NUM 1
#if USE_DHCP
  if (0==Ethernet.begin(mac)) {
#endif
    LOGLN("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac, ip/*, {DNS}, gateway, subnet*/);
#if USE_DHCP
  }
#endif
  // then don't forget Ethernet.maintain()
  LOG("IP:   ");  LOGLN(Ethernet.localIP());
  LOG("MASK: ");  LOGLN(Ethernet.subnetMask());
  LOG("GATE: ");  LOGLN(Ethernet.gatewayIP());
  LOG("DNS:  ");  LOGLN(Ethernet.dnsServerIP());
  wsengine.begin();
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
#if USE_ETH
    // no true statement since half-maxlife
    if (timer > (((uint8_t)-1)>>1)) {
      LOGLN("*** new time cycle ***");
#if USE_DHCP
      Ethernet.maintain(); /* added in 1.0.1 - default Ubuntu IDE is still in 1.0 */
      LOGLN("renew DHCP");
#endif
      // OR: software_reset();
    } else {
      LOGLN("*** new time section ***");
    }
    // DO SOMETHING NEW
#endif
#if BUSYLED_PIN
  WAIT(2000);
  digitalWrite(BUSYLED_PIN, LOW);
#endif
  } // if (timer) 
#if USE_ETH
  wsengine.check();
#endif
}

