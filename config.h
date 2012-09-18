#ifndef CONFIG_H_
#define CONFIG_H_


#define DEBUG 1

// LED
#define BUSYLED_PIN 13

// USB conf
#if defined(__AVR_ATmega1280__)
#define USB_SPEED   9600
#else
#define USB_SPEED   115200
#endif

// ETH conf
#define USE_DHCP    0
#define IP          10,240,170, 12
#define SUBNET      255,255,255,0
#define GATEWAY     0,0,0,0
#define DNS         0,0,0,0
// ascii code for "@lan#" + HEX 12
#define MAC         0x40,0x6C,0x61,0x6E,0x23, 0x0C
#define PORT        80
#define ETH_BLPIN   53 /* power of W5100 chip */

// LCD conf
#define LCD_PINS            8, 13, 9, 4, 5, 6, 7
#define LCD_BLPIN           10
#define LCD_BLPOLARITY      POSITIVE // or POSITIVE, see ./library/LCD.h tpol, t_backlighPol
#define LCD_ABTN            0 // analog pin of buttons
#define LCD_WIDTH           16
#define LCD_HEIGHT          2
#define LCDPAD_LATENCY1_MS  10    // detect press down
#define LCDPAD_LATENCYX_MS  500   // switch mode from ONCE to MULTI
#define LCDPAD_SPEED_MS     200   // considering new press down


// VELOCITY (choose inside macros.h)
//#define EMBEDTIME EMBEDTIME_65s_4h
#define EMBEDTIME EMBEDTIME_16s_1h

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
