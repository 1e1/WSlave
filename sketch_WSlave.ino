/*
  Web Server
 
 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)
 
 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 
 */


#define DEBUG 1

#include "config.h"
#include "macros.h"
#include "WSlave.h"
#include <SPI.h>


/** configure that **/
static intfDigital digitals[] = {
  { 22, "relay1.1" },
  { 24, "relay1.2" },
  { 26, "relay1.3" },
  { 28, "relay1.4" },
  { 30, "relay1.5" },
  { 32, "relay1.6" },
  { 34, "relay1.7" },
  { 36, "relay1.8" }
};
static intfPulse pulses[] = {
  { 44, 0, "Pulse" }
};
static byte temperaturePins[] = { 13 };
static intfMessage messages[] = {
  { obsTemperature, 'x', temperaturePins, "Indoor" }
};


#if defined(LCD_PINS) && defined(LCD_WIDTH) && defined(LCD_HEIGHT) && defined(LCD_BLPIN)
#include <LiquidCrystal.h>
static LiquidCrystal lcd = LiquidCrystal(LCD_PINS);
static char lcdLines[LCD_HEIGHT][LCD_WIDTH];
#define USE_LCD 1
#else
#define USE_LCD 0
#endif

#if defined(IP) && defined(MAC) && defined(PORT) && defined(GATEWAY) && defined(SUBNET) 
#include <Ethernet.h>
static byte mac[] = { MAC };
static IPAddress ip(IP);
static IPAddress gateway(GATEWAY);
static IPAddress subnet(SUBNET);
static EthernetServer server(PORT);
// last address
#include "webApp.h"
#define USE_ETH 1
#else
#define USE_ETH 0
#endif



/** ===================== **/
/**       observers       **/
/** ===================== **/

char obsTemperature (byte *pins)
{
  return 'x';
}

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
  if (0==Ethernet.begin(mac)) {
    LOGLN("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac, ip/*, {DNS},*/, gateway, subnet);
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
    client.stop();
    LOGLN("client disonnected");
  }
}

