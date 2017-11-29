/* slave address 1 - 255 */
#define DIR 0x05    //slave dir 5d

/* data types  */
typedef unsigned char   uint8;
typedef unsigned int    uint16;
typedef unsigned long   uint32;

/* data range */
#define MAX_COILS 1
#define MAX_INPUTS 1
#define MAX_HOLDING_REGISTERS 2
#define MAX_INPUTS_REGISTERS 2

/* internal slave memory data */
uint8 idata coils_array[MAX_COILS]; 
uint8 idata inputs_array[MAX_INPUTS];
uint16 idata holding_registers_array[MAX_HOLDING_REGISTERS];
uint16 idata inputs_registers_array[MAX_INPUTS_REGISTERS];

/* functions prototypes */
uint8 GetCoilValue(uint16 adr);
void SetCoilValue(uint16 adr, uint8 value);
uint8 GetInputValue(uint16 adr);
uint16 GetHoldingRegisterValue(uint16);
void SetHoldingRegisterValue(uint16, uint16);
uint16 GetInputRegisterValue(uint16);

/* functions codes */
/* coils read-write operations */
uint8 GetCoilValue(uint16 adr)
{
    uint8 res = 0, i = 0, d = 0;
    if ( adr < 0 || adr >= MAX_COILS * 8 )
    {
        return 0;
    } 
    i = adr / 8;
    d = adr % 8;    
    res = ( coils_array[i] & (1 << d) );
    return res > 0;
}

void SetCoilValue(uint16 adr, uint8 value)
{
    uint8 res = 0, i = 0, d = 0;    
    if ( adr < 0 || adr >= MAX_COILS * 8 )
    {
        return ;
    }
    i = adr / 8;
    d = adr % 8;
    res = ( coils_array[i] & (1 << d) ); 
    if ( res > 0 ) 
    {
        res = 1;    //coil on
    }
    else 
    {
        res = 0;    //coil off
    }
    if ( res == 1 && value == 1 ) return;  // already ON
    if ( res == 0 && value == 0 ) return;  // already OFF    
    if ( res == 0 && value == 1 )        // is off turn on
    {
        coils_array[i] = ( coils_array[i] | (1 << d) );     // active coil           
    }
    if ( res == 1 && value == 0 )        // is on turn off
    {        
        coils_array[i] = ( coils_array[i] & ( ~(1 << d) ) );     // desactive coil       
    }    
    return ;
}

/* holding registers read-write operations */
uint16 GetHoldingRegisterValue(uint16 adr)
{   
    if ( adr < 0 || adr >= MAX_HOLDING_REGISTERS )
    {
        return 0;   // invalid register address  
    }
    else
    {
        return holding_registers_array[adr];
    }
}

void SetHoldingRegisterValue(uint16 adr, uint16 value)
{
    if ( adr < 0 || adr >= MAX_HOLDING_REGISTERS || value < 0 || value > 65535)
    {
        return ;   // invalid register address  
    }
    else
    {
        holding_registers_array[adr] = value;
    }
}

/* inputs read-only operations */
uint8 GetInputValue(uint16 adr)
{    
    uint8 res = 0, i = 0, d = 0;    
    if ( adr < 0 || adr >= MAX_INPUTS * 8 )
    {
        return 0;
    }
    i = adr / 8;
    d = adr % 8;    
    res = ( inputs_array[i] & (1 << d) );
    return res > 0;
}

uint16 GetInputRegisterValue(uint16 adr)
{
    if ( adr < 0 || adr >= MAX_INPUTS_REGISTERS )
    {
        return 0;   // invalid register address  
    }
    else
    {      
        return inputs_registers_array[adr];
    }
}
