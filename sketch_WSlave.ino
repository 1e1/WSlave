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


#define DEBUG 1

#include "config.h"
#include "macros.h"
#include "WSlave.h"
#include "FastTimer.h"
#include <SPI.h>



/** ===================== **/
/**      connections      **/
/** ===================== **/

static intfDigital digitals[] = {
  WSDIGITAL("relay1.1", 22),
  WSDIGITAL("relay1.2", 24),
  WSDIGITAL("relay1.3", 26),
  WSDIGITAL("relay1.4", 28),
  WSDIGITAL("relay1.5", 30),
  WSDIGITAL("relay1.6", 32),
  WSDIGITAL("relay1.7", 34),
  WSDIGITAL("relay1.8", 36)
};
static intfPulse pulses[] = {
  WSPULSE("Pulse", 24)
};
byte temperaturePins[] = { 13 };
static intfMessage messages[] = {
  { obsTemperature, 'x', temperaturePins, "Indoor" }
};

/** ===================== **/
/**       observers       **/
/** ===================== **/

char obsTemperature (byte *pins)
{
  return 'x';
}

/** ===================== **/

//inline unsigned int embedTime() __attribute__((always_inline));
//inline unsigned int embedTime()
//{
//  return (unsigned int) (millis() >> 12);
//}

// Restarts program from beginning but does not reset the peripherals and registers
void software_Reset()
{
  asm volatile ("jmp 0");  
}




#if defined(LCD_PINS) && defined(LCD_WIDTH) && defined(LCD_HEIGHT) && defined(LCD_BLPIN)
#include <LiquidCrystal.h>
LiquidCrystal lcd = LiquidCrystal(LCD_PINS);
char lcdLines[LCD_HEIGHT][LCD_WIDTH];
#define USE_LCD 1
#else
#define USE_LCD 0
#endif

#if defined(IP) && defined(MAC) && defined(PORT) && defined(GATEWAY) && defined(SUBNET) 
#include <Ethernet.h>
byte mac[] = { MAC };
IPAddress ip(IP);
/*
IPAddress gateway(GATEWAY);
IPAddress subnet(SUBNET);
*/
EthernetServer server(PORT);
// last address
#include "webApp.h"
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
  if (0==Ethernet.begin(mac)) {
    LOGLN("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac, ip/*, {DNS}, gateway, subnet*/);
  }
  // then don't forget Ethernet.maintain()
  LOG("IP:   ");  LOGLN(Ethernet.localIP());
  LOG("MASK: ");  LOGLN(Ethernet.subnetMask());
  LOG("GATE: ");  LOGLN(Ethernet.gatewayIP());
  LOG("DNS:  ");  LOGLN(Ethernet.dnsServerIP());
  server.begin();
  LOGLN("server started");
#endif
}


void loop()
{
  if (FastTimer::update()) {
#if USE_ETH
    if (FastTimer::isNewCycle()) {
      Ethernet.maintain();
      LOGLN("renew DHCP");
    }
    // DO SOMETHING NEW
#endif
  }
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    LOGLN("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        LOG(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (currentLineIsBlank && '\n' == c) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connnection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
                    // add a meta refresh tag, so the browser pulls again every 5 seconds:
          client.println("<meta http-equiv=\"refresh\" content=\"5\">");
          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");       
          }
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if ('\r' != c) {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.flush();
    client.stop();
    LOGLN("client disonnected");
  }
}

