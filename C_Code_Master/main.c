#include <stdio.h>
#include <main.h>
#include <master.h>


int word = 0; 
int i = 0;
int coil = 0;


void Serial_Init()
{ 
    RS485En = 0; // set for rx

    TMOD = 0x20; 
    TH1 = TIMER_HIGHT;
    TL1 = TIMER_LOW;  //2400 BAUDS
    TR1 = 1;  
    SCON = 0x50;    //mode 1 8 bits 1 start, 8 data, 1 stop 
    
    ES = 1;
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
void tx_byte(char byte)     //transmit a single byte
{      
    TI = 0;
    TB8 = 0;
    SBUF = byte;
    while (!TI);
}

void SerialPortInt() interrupt 4
{
    if( TI == 1 )
    {       
        TI = 0;
        if ( RS485En == 0 ) return;
            
        if ( send_count < data_count )
        {         
            delay_ms(100);        
            SBUF = ascii_frame[send_count++];     
           
        }
        else
        {                                
            clear_frame();          
        }
    }
    else                 
    if( RI == 1 )
    {
        RI = 0;
    }
}

void main(void)
{        
    Serial_Init();
   
    word = 0;   
    EA = 1;
    delay_ms(100000);
     
    while (1)
    {            
        PresetSingleRegister_06(5, 1, word);
        word++;
        if ( word == 20 ) word = 0;
        
        delay_ms(100000);    
       
        
        // ForceSingleCoil_05(5, 1, coil);
        
        
        if ( coil == 0 ) coil = 1;
            else coil = 0;
            
        delay_ms(100000);
        PresetSingleRegister_06(5, 2, 5);
        
     
       delay_ms(100000);
    }    
}