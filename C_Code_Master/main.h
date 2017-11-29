#include "reg52.h"
 
/* data types */
typedef unsigned char   uint8;
typedef unsigned int    uint16;
typedef unsigned long   uint32;

/* RS485 enable/disable terminal */
sbit RS485En  =  P2^7;
     
/* timer conf for uart baudrate */
#define TIMER_HIGHT 0xF4
#define TIMER_LOW   0xF4    //2400


void delay_ms(unsigned int d);