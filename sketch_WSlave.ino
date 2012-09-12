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
const uint8_t messages_len = ARRAYLEN(messages);

/** ===================== **/

// Restarts program from beginning but does not reset the peripherals and registers
__attribute__((always_inline)) inline void software_reset();
void software_reset()
{
  asm volatile ("jmp 0");
}




#if defined(USB_SPEED) && !DEBUG
#include "USlave.h"
#define USE_USB 1
#else
#define USE_USB 0
#endif

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
#define USE_ETH 1
#else
#define USE_ETH 0
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
#if USE_LCD
  lcd.begin(LCD_WIDTH, LCD_HEIGHT);
  pinMode(LCD_BLPIN, OUTPUT);
  digitalWrite(LCD_BLPIN, HIGH);
#endif
#if USE_ETH
  // reduce DHCP timeout, default is 60000ms
  // change: Ethernet.cpp/EthernetClass::begin{...int ret = _dhcp->beginWithDHCP(mac_address);...}
  // by:     Ethernet.cpp/EthernetClass::begin{...int ret = _dhcp->beginWithDHCP(mac_address, 10000);...}
  // reduce number of client sockets to only one master (break loops)
  // change: Ethernet.h/#define MAX_SOCK_NUM 4
  // by:     Ethernet.h/#define MAX_SOCK_NUM 1
  WSlave::begin();
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
      WSlave::maintain();
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
  
  // CHECK
#if USE_ETH
  WSlave::check();
#endif
#if USE_USB
  USlave::check();
#endif
  
  // UNCHECK
#if USE_ETH
  WSlave::uncheck();
#endif
}

