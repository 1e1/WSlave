#ifndef MACROS_H_
#define MACROS_H_


/** SETUP ALIAS **/
#define WSDIGITAL(NAME, PIN) { PIN, NAME }
#define WSPULSE(NAME, PIN) { PIN, 0, NAME }
#define WSMESSAGE(NAME, FUNCTION, PINS...) { FUNCTION, '#', {PINS}, NAME }
/** === **/



/** DEBUGGING TOOLS **/
#if DEBUG
  #define LOG(message)    Serial.print  (message);
  #define LOGLN(message)  Serial.println(message);
#else
  #define LOG(message)
  #define LOGLN(message)
#endif
/** === **/



/** TIME **/
//==>  >> 0: 1 unit of embedTime is 0.001s
//-->  const unsigned long maxTime = 4294967295; // = 49d 17h 02m 47s
//-->  const unsigned int maxTime = 65535; // = 65s
//==>  >> 12: 1 unit of embedTime is 4.096s
//-->  const unsigned int maxEmbedTime = 65535; // = 3d 02h 33mn 51s 360ms
//     const unsigned int moduloEmbedTime = 63281; // = 3d
//-->  const uint8_t maxEmbedTime = 255; // = 17mn 24s 480ms
//     const uint8_t moduloEmbedTime = 219; // = 15mn 00s 000ms = 219.72991531
//==>  >> 14: 1 unit of embedTime is 16.384s
//-->  const uint8_t maxEmbedTime = 255; // = 1h 09mn 37s 920ms
//     const uint8_t moduloEmbedTime = 219; // = 1h 00mn 00s 000ms = 219.72991531
//==>  >> 16: 1 unit of embedTime is 65.535s
//-->  const uint8_t maxEmbedTime = 255; // = 4h 38mn 31s 425ms
//     const uint8_t moduloEmbedTime = 219; // = 4h 00mn 00s 000ms = 219.72991531
#define EMBEDTIME_4s_15m   ((uint8_t) (millis() >> 12))
//#define EMBEDTIME_4s_3d    ((unsigned int) (millis() >> 12))
#define EMBEDTIME_16s_1h   ((uint8_t) (millis() >> 14))
#define EMBEDTIME_65s_4h   ((uint8_t) (millis() >> 16))
/** === **/



/** FAST TRIGO **/
#define FACTOR_PI_UINT12(x) (50*x) >> 4
#define FACTOR_PI_UINT8(x) (804*x) >> 8
/** === **/



/** LOOP **/
// wikipedia "foreach_loop"
#define FOREACH(idxtype, idxpvar, col, colsiz ) idxtype* idxpvar; for( idxpvar=col ; idxpvar < (col + (colsiz)) ; idxpvar++)
#define ARRAYLEN( ary ) ( sizeof(ary)/sizeof(ary[0]) )
/*
FOREACH (char, p1, c1, strlen(c1) ) {
  printf("loop 1 : %c\n",*p1);
}
FOREACH (int, p2, c2, ARRAYLEN(c2) ){
  printf("loop 2 : %d\n",*p2);
}
*/
/** === **/


#endif // MACROS_H_
