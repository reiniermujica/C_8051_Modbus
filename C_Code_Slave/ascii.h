/* data types  */
typedef unsigned char   uint8;
typedef unsigned int    uint16;
typedef unsigned long   uint32;

/* ascii temp vars */
uint8 ascii[4];
uint8 hexa[4];

/* functions prototypes */
void ByteToAscii(uint8);
uint8 NibbleToAscii(uint8);
uint8 AsciiToByte(uint8, uint8);
void TwoByteToAscii(uint16);
uint16 AsciiToTwoByte();

/* functions code */
void ByteToAscii(uint8 val)
{
    uint8 a = val / 16; 
    uint8 b = val % 16;       
    if ( a <= 9 )        
    {
        ascii[0] = a + '0';
    }
    else
    {
        ascii[0]= (a - 10) + 'A';    
    }    
    if ( b <= 9 )        
    {
        ascii[1] = b + '0';
    }
    else
    {
        ascii[1]= (b - 10) + 'A';    
    }
    return ;
}

uint8 NibbleToAscii(uint8 val)
{     
    if ( val <= 9 )        
    {
        return val + '0';
    }
    else
    {
        return (val - 10) + 'A';    
    }        
}

uint8 AsciiToByte(uint8 a, uint8 b)
{
    if ( a >= 65 )        
    {
        a = a - 'A' + 10;
    }
    else
    {
        a = a - '0';    
    }    
    if ( b >= 65 )        
    {
        b = b - 'A' + 10;
    }
    else
    {
        b = b - '0';    
    }       
    return (16 * a) + b;
}

void TwoByteToAscii(uint16 val)
{
    int i = 0;
    for ( i = 0; i < 4; i++ )
        hexa[i] = 0;    
    i = 0;
    while ( val > 0)
    {
        hexa[i] = val % 16;
        val /= 16;    
        i++; 
    }            
    for ( i = 0; i < 4; i++ )
        ascii[i] = NibbleToAscii(hexa[i]);    
    return ;
}

uint16 AsciiToTwoByte()
{
    uint16 a = AsciiToByte(ascii[0], ascii[1]);
    uint16 b = AsciiToByte(ascii[2], ascii[3]);
    uint16 result = ( a * 256 ) + b;
    return result;
}
