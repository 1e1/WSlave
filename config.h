#ifndef CONFIG_H_
#define CONFIG_H_


// a number between 1 and 254
#define DEVICE_NUMBER  12
#define DEVICE_ID     "12"
#define EMAIL         "email@domain.com"
#define ML_SUBJECT    "[Wslave#" DEVICE_ID "]"


#define DEVICE_NAME "WSlave_" DEVICE_ID
#define DEBUG 0

// LED
#define BUSYLED_PIN 13

// USB conf
#define USB_SPEED   115200

// ETH conf
#define USE_BONJOUR     0
#define USE_DHCP        1
#define IP              10,240,170, DEVICE_NUMBER
#define SUBNET          255,255,255,0
#define GATEWAY         0,0,0,0
#define DNS             0,0,0,0
// ascii code for "@lan#" + HEX 12
#define MAC             0x40,0x6C,0x61,0x6E,0x23, DEVICE_NUMBER
#define PORT            80
#define ETH_BLPIN       53 /* power of W5100 chip */
#define HTTP_AUTH64     "YXJkbWluOkBsYW4jMTI=" // base64("ardmin:@lan#12"); /!\ chars must be parsed by reading buffer
#define SMTP_IP         173,194,67,108 // smtp.gmail.com
#define SMTP_PORT       25
// 0.fr.pool.ntp.org
#define NTP_SERVER      /* 192, 168, 170, 254 */ 88, 191, 80, 53 // or use GATEWAY if not specified
#define NTP_PORT        123
#define NTP_LOCALPORT   3669 /* OR "ODE 8400" */
#define NTP_PACKET_SIZE 48
#define TZ_OFFSET       (+1) /* no DST by default (case of 12/31/y and 01/01/y+1)  */
#define TZ_DST          1

// LCD conf
#define LCD_PINS            8, 13, 9, 4, 5, 6, 7
//#define LCD_BLPIN           10
#define LCD_BLPOLARITY      POSITIVE // or POSITIVE, see ./library/LCD.h tpol, t_backlighPol
#define LCD_ABTN            0 // analog pin of buttons
#define LCD_WIDTH           16
#define LCD_HEIGHT          2
#define LCDPAD_LATENCY1_MS  10    // detect press down
#define LCDPAD_LATENCYX_MS  500   // switch mode from ONCE to MULTI
#define LCDPAD_SPEED_MS     200   // considering new press down

#define KEYPAD_MAXVALUE_RIGHT       /* */ 30  /* * / 50  /* */
#define KEYPAD_MAXVALUE_UP          /* */ 150 /* * / 200 /* */
#define KEYPAD_MAXVALUE_DOWN        /* */ 360 /* * / 400 /* */
#define KEYPAD_MAXVALUE_LEFT        /* */ 535 /* * / 600 /* */
#define KEYPAD_MAXVALUE_SELECT      /* */ 760 /* * / 800 /* */


// VELOCITY (choose inside macros.h)
//#define EMBEDTIME EMBEDTIME_65s_4h
//#define EMBEDTIME EMBEDTIME_16s_1h
#define EMBEDTIME EMBEDTIME_1s_4m

/*
// design conf
//#define PIN_D00 "RX"
//#define PIN_D01 "TX"
//#define PIN_D02 "-0*"
//#define PIN_D03 "-1*"
//#define PIN_D04 "*"
//#define PIN_D05 "*"
//#define PIN_D06 "*"
//#define PIN_D07 "*"
//#define PIN_D08 "*"
//#define PIN_D09 "*"
//#define PIN_D10 "*"
//#define PIN_D11 "*"
//#define PIN_D12 "*"
//#define PIN_D13 "*(LED_BUILTIN)"
//#define PIN_D14 "TX3"
//#define PIN_D15 "RX3"
//#define PIN_D16 "TX2"
//#define PIN_D17 "RX2"
//#define PIN_D18 "TX1-5"
//#define PIN_D19 "RX1-4"
//#define PIN_D20 "SDA-3"
//#define PIN_D21 "SCL-2"
#define PIN_D22          "relay1.1"
#define PIN_D23          "relay2.1"
#define PIN_D24          "relay1.2"
#define PIN_D25          "relay2.2"
#define PIN_D26          "relay1.3"
#define PIN_D27          "relay2.3"
#define PIN_D28          "relay1.4"
#define PIN_D29          "relay2.4"
#define PIN_D30          "relay1.5"
#define PIN_D31          "relay2.5"
#define PIN_D32          "relay1.6"
#define PIN_D33          "relay2.6"
#define PIN_D34          "relay1.7"
#define PIN_D35          "relay2.7"
#define PIN_D36          "relay1.8"
#define PIN_D37          "relay2.8"
#define PIN_D38
#define PIN_D39
#define PIN_D40
#define PIN_D41
#define PIN_D42
#define PIN_D43
#define PIN_D44 "*"
#define PIN_D45 "*"
#define PIN_D46 "*"
#define PIN_D47
#define PIN_D48
#define PIN_D49
//#define PIN_D50 "MISO"
//#define PIN_D51 "MOSI"
//#define PIN_D52 "SCK"
//#define PIN_D53 "SS"
//#define PIN_D54 "undefined"
//#define PIN_D55 "undefined"
//#define PIN_D56 "undefined"
//#define PIN_D57 "undefined"
//#define PIN_D58 "undefined"
//#define PIN_D59 "undefined"

#define PIN_A00
#define PIN_A01
#define PIN_A02          "Window5°L"   // lux #5 sensor
#define PIN_A03          "Window4°L"   // lux #4 sensor
#define PIN_A04          "Window3°L"   // lux #3 sensor
#define PIN_A05          "Window2°L"   // lux #2 sensor
#define PIN_A06          "Window1°L"   // lux #1 sensor
#define PIN_A07          "Room5°C"     // bedroom #5 sensor
#define PIN_A08          "Room4°C"     // bedroom #4 sensor
#define PIN_A09          "Room3°C"     // bedroom #3 sensor
#define PIN_A10          "Room2°C"     // bedroom #2 sensor
#define PIN_A11          "Room1°C"     // bedroom #1 sensor
#define PIN_A12          "Bath°C"      // bathroom sensor
#define PIN_A13          "Living°C"    // living room sensor
#define PIN_A14          "SOutdoor°C"  // south sensor
#define PIN_A15          "NOutdoor°C"  // north sensor
//#define PIN_A16 "undefined"
//#define PIN_A17 "undefined"
//#define PIN_A18 "undefined"
//#define PIN_A19 "undefined"
*/

#endif
