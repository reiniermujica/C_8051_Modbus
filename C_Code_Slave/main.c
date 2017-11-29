#include <stdio.h>
#include <main.h>
#include <modbus.h>
#include <reg52.h>

#define MMOD 1009

int level = 0;
int cant = 1;
int led_count = 0;
int sec_count = 0;

int last_data_count = 0;
int last_hash       = 0;

uint8 idata _7seg[10] = {0x7E,0x42,0xEC,0xE6,0xD2,0xB6,0x9E,0x62,0xFE,0xF2};

sbit LED1 = P1^3;
sbit EN_D = P3^7;
sbit ONE  = P2^0;
uint8 BUSY = 0;

void Serial_PWM_Init()
{ 
    RS485En = 0;

    TMOD = 0x21;    // 

    TH1 = TIMER_HIGHT_1;
    TL1 = TIMER_LOW_1;  //2400 BAUDS

    TH0 = TIMER_HIGHT_0;  // 2 ms
    TL0 = TIMER_LOW_0;

    SetHoldingRegisterValue(1, 1);  // level 5 full bright
    level = 5;
   
    ET0 = 1;
    TR0 = 1;

    SCON = 0x50;    //mode 1 8 bits 1 start, 8 data, 1 stop 
    
    ES = 1;
    PS = 1;
    PT1 = 1;

    TR1 = 1; 
}

int make_hash() 
{
    int i, h = 0;
    for ( i = 0; i < data_count; i++ ) {
        h += (char)ascii_frame[i] * ( i + 1 );
    }
    return ( 19 * h ) % MMOD;
}

void delay_ms(unsigned int d)
{
    int k = 0;
    int s = 0;
    for ( k = 0; k < d; k++ )
    {     
        s++;
    }
}

void PWM() interrupt 1
{
    int curr = 0;
    int off = GetCoilValue(0);
    
    TH0 = TIMER_HIGHT_0;  
    TL0 = TIMER_LOW_0;
    
    if ( cant <= 0 )
    {
        EN_D = DISPLAY_OFF;
    }
    else
    {         
        EN_D = !(DISPLAY_ON ^ off);
    }           
    
    if ( cant >= level )
    {     
        cant = level - 5;
    }
    
    cant++;

    led_count++;
    if (led_count == 500)   // 1 second
    {
        led_count = 0;
        LED1 = ~LED1;         
        
      /*  curr = make_hash();
    
        if ( last_hash == curr )
        {
            clear_frame();
            last_hash = curr = 0;        
        }
        last_hash = curr;        */        
    }

    TF0 = 0;   
    TR0 = 1;
}

void SerialPortInt() interrupt 4
{
    uint8 rx = 0;

    if( TI == 1 )
    {       
        TI = 0;
        if ( RS485En == 0 ) 
        {            
            return;
        }
            
        if ( send_count < data_count )
        {                       
            TB8 = 0;                    
            SBUF = ascii_frame[send_count];        
            send_count++;
        }
        else
        {                      
            RS485En = 0;    //rx
            clear_frame();
        }

    }
    else                 
    if( RI == 1 )
    {
	
        if ( RS485En == 0 && BUSY == 1 )	// rx and not busy
        {           
            rx = SBUF;             
            RI = 0;
            if ( rx == 0 ) return;  //discard bad characters
                
            if ( data_count == 0 )
            {             
                if ( rx == ':' )    // new frame
                {                                  
                    ascii_frame[data_count++] = rx;    
                }            
            }
            else  
            {
                ascii_frame[data_count++] = rx;     

                if ( data_count >= 9 )
                {                    
                    if ( (ascii_frame[data_count-1] == LF) && (ascii_frame[data_count-2] == CR) )
                    {                            
                        BUSY = 0;   // device busy = true

                        execute_modbus_command();
                    
                        BUSY = 1;   // device busy = false
                    }
                }                      
                if ( data_count == ASCII_FRAME_SIZE )   // bad frame size
                {                
                    clear_frame();             
                }                                                       
            }
        }
        else
            RI = 0;
    }
}

void update_display()
{    
    int x = (int)GetHoldingRegisterValue(0); // number             
    int L = (int)GetHoldingRegisterValue(1); // bright

    if ( x >= 0 && x <= 9 )
    {       
        P2 = _7seg[x]; 
        ONE = 0;    //off 1
    }      
    else
    {
        if ( x < 20 )
        {            
            x = x % 10;
            P2 = _7seg[x]; 
            ONE = 1;                    
        }   
    }
  
    if ( L >= 0 && L <= 5 )
        level = L;
}

void main(void)
{        
    Serial_PWM_Init(); 
    
    SetCoilValue(0,1);        
    SetHoldingRegisterValue(0,1);      

    EN_D = DISPLAY_OFF;
    LED1 = 1;
    RS485En = 0;
    BUSY = 1;

    clear_frame();

    EA = 1;    
   
    while (1)
    {                     
        update_display();
        delay_ms(1000);
    }    
}