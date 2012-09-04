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
