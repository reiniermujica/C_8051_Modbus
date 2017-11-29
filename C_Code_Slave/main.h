#include "reg52.h"
 
/* data types */
typedef unsigned char   uint8;
typedef unsigned int    uint16;
typedef unsigned long   uint32;

/* RS485 enable/disable terminal */
sbit RS485En  =  P3^2;
     
/* timer conf for uart baudrate */
#define TIMER_HIGHT_1 0xF4
#define TIMER_LOW_1   0xF4      // 2400

/* timer conf for PWM */
#define TIMER_HIGHT_0 0xF7      //2ms conf
#define TIMER_LOW_0   0x86

/* Display on / off states */
#define DISPLAY_ON 0
#define DISPLAY_OFF 1

void delay_ms(unsigned int d);