#include <slave.h>
#include <ascii.h>

/* ASCII FRAME size */
#define ASCII_FRAME_SIZE 60   //CHANGE  

/* ASCII FRAME END CHARS */
#define CR 0x0D
#define LF 0x0A         

/* ASCII FRAME */
uint8 idata ascii_frame[ASCII_FRAME_SIZE]; // max size = 255
uint8 idata data_count = 0;
uint8 idata send_count = 0;
bit broadcast = 0;

/* functions prototypes*/
void execute_modbus_command();
void gen_lrc();
void clear_frame();
uint8 lrc_calc();
void tx_assci_frame();

void ResponseReadCoilsStatus_01();
void ResponseReadInputStatus_02();
void ResponseReadHoldingRegisters_03();
void ResponseReadInputRegisters_04();
void ResponseForceSingleCoil_05();
void ResponsePresetSingleRegister_06();
void ResponseForceMultipleCoils_15();
void ResponsePresetMultipleRegisters_16();

void execute_modbus_command()   
{
    uint8 is_me = 0, fun = 0, lrc = 0;         
    if ( data_count < 9 )
    {     
        clear_frame();  // bad frame discard , minimun 9 characters for valid  
        return;
    }
    data_count -= 2; //skip cr lf       
    lrc = AsciiToByte(ascii_frame[data_count-2], ascii_frame[data_count-1]);
    data_count -= 2; //skip lrc   
    fun = lrc_calc(); 
    if ( fun != lrc )
    {     
        clear_frame();  // bad LRC, frame discard    
        return;
    }
     
    /*   frame ok  */
    is_me = AsciiToByte(ascii_frame[1], ascii_frame[2]); 
    if ( is_me == 0 )
    {
        broadcast = 1;    
    }
    else
    {
        broadcast = 0;
        if ( DIR != is_me )
        {
            clear_frame();
            return;   
        }
    }

    fun = AsciiToByte(ascii_frame[3], ascii_frame[4]);    
    switch ( fun )
    {
        case 1:
        {                 
            ResponseReadCoilsStatus_01();            
            break;        
        }
        case 2:
        {
            ResponseReadInputStatus_02();
            break;
        }
        case 3:
        {        
            ResponseReadHoldingRegisters_03();
            break;
        }
        case 4:
        {
            ResponseReadInputRegisters_04();
            break;
        }
        case 5:
        {           
            ResponseForceSingleCoil_05();            
            break;
        }
        case 6:
        {
            ResponsePresetSingleRegister_06();            
            break;
        }
        case 15:
        {                     
            ResponseForceMultipleCoils_15();
            break;            
        }        
        case 16:
        {              
            ResponsePresetMultipleRegisters_16();
            break;            
        }        
        default: 
        {
            break;
        }
    }
}

void gen_lrc()
{
    ByteToAscii(lrc_calc());
    ascii_frame[data_count++] = ascii[0];
    ascii_frame[data_count++] = ascii[1];
    
    ascii_frame[data_count++] = CR;
    ascii_frame[data_count++] = LF;    
}
void clear_frame()
{    
    uint8 i = 0;
    data_count = 0;
    send_count = 0;
    for ( i = 0; i < ASCII_FRAME_SIZE; i++ )
    {
        ascii_frame[i] = 0;
    }
}
void tx_assci_frame()
{
    if ( broadcast == 1 )
    {
        RS485En = 0;    //rx
        broadcast = 0;
        clear_frame();
        return ;    
    }
   
    RS485En = 1; // set for tx   
    send_count = 1;
    TI = 0;
    TB8 = 0;        
    SBUF = ascii_frame[0];
}
uint8 lrc_calc() 
{
    uint8 result = 0, i = 0;            
    for ( i = 1; i < data_count; i+=2 )
    {
        result += AsciiToByte(ascii_frame[i], ascii_frame[i+1]);
    }
    result = ~result;
    return ( result + 1 );
}

/* function codes */
void ResponseReadCoilsStatus_01() // 0x01, OK
{
    uint16 start = 0, cant = 0;
    uint16 i = 0, limit = 0;
    uint8 coils = 0, k = 0, new_data_count = 0;
    data_count = 7;  // data count byte reserved        
    for ( i = 0; i < 4; i++ )
    {
        ascii[i] = ascii_frame[i + 5];
    }    
    start = AsciiToTwoByte();
    for ( i = 0; i < 4; i++ )
    {
        ascii[i] = ascii_frame[i + 9];
    }    
    cant = AsciiToTwoByte();    
    limit = start + cant;

    /* querying coils status */
    for ( i = start; i < limit; i += 8 )
    {
        k = 0;
        coils = 0;    
        while ( k < 8 && i + k < limit)            
        {        
            uint8 val = GetCoilValue(i + k);    // status of coil i+k
            if ( val == 1 ) //coil active
            {
                coils |= (1 << k);            
            }                
            k++;
        }            
        ByteToAscii(coils);
        ascii_frame[data_count++] = ascii[0];
        ascii_frame[data_count++] = ascii[1];        
        new_data_count++;     // add one byte to response frame
    }
    
    ByteToAscii(new_data_count);
    ascii_frame[5] = ascii[0];
    ascii_frame[6] = ascii[1];

    gen_lrc();
    
    tx_assci_frame();
    return;
}
void ResponseReadInputStatus_02() // 0x02, OK!!!
{
    uint16 start = 0, cant = 0;
    uint16 i = 0, limit = 0;
    uint8 inputs = 0, k = 0, new_data_count = 0;
    data_count = 7;  // data count byte reserved        
    for ( i = 0; i < 4; i++ )
    {
        ascii[i] = ascii_frame[i + 5];
    }    
    start = AsciiToTwoByte();
    for ( i = 0; i < 4; i++ )
    {
        ascii[i] = ascii_frame[i + 9];
    }
    
    cant = AsciiToTwoByte();    
    limit = start + cant;

    /* querying inputs status */
    for ( i = start; i < limit; i += 8 )
    {
        k = 0;
        inputs = 0;    
        while ( k < 8 && i + k < limit)            
        {        
            uint8 val = GetInputValue(i + k);    // status of input i+k
            if ( val == 1 ) //input active
            {
                inputs |= (1 << k);            
            }                
            k++;
        }            
        ByteToAscii(inputs);
        ascii_frame[data_count++] = ascii[0];
        ascii_frame[data_count++] = ascii[1];
        
        new_data_count++;     // add one byte to response frame
    }
    
    ByteToAscii(new_data_count);
    ascii_frame[5] = ascii[0];
    ascii_frame[6] = ascii[1];

    gen_lrc();
    
    tx_assci_frame();
    return;
}
void ResponseReadHoldingRegisters_03()  // 0x03, OK!!!
{
    uint16 start = 0, cant = 0;
    uint16 i = 0, limit = 0, status = 0;
    uint8 new_data_count = 0;
    data_count = 7;  // data count byte reserved        
    for ( i = 0; i < 4; i++ )
    {
        ascii[i] = ascii_frame[i + 5];
    }    
    start = AsciiToTwoByte();
    for ( i = 0; i < 4; i++ )
    {
        ascii[i] = ascii_frame[i + 9];
    }    
    cant = AsciiToTwoByte();    
    limit = start + cant;

    /* querying holding registers status */
    for ( i = start; i < limit; i++ )
    {
        status = GetHoldingRegisterValue(i);       
        TwoByteToAscii(status);
         
        ascii_frame[data_count++] = ascii[3];
        ascii_frame[data_count++] = ascii[2];
        ascii_frame[data_count++] = ascii[1];
        ascii_frame[data_count++] = ascii[0];                          
    
        new_data_count+=2;     // add two bytes to response frame
    }
    
    ByteToAscii(new_data_count);
    ascii_frame[5] = ascii[0];
    ascii_frame[6] = ascii[1];

    gen_lrc();
    
    tx_assci_frame();
    return;
}
void ResponseReadInputRegisters_04()  // 0x04,  OK!!!!
{
    uint16 start = 0, cant = 0;
    uint16 i = 0, limit = 0, status = 0;
    uint8 new_data_count = 0;
    data_count = 7;  // data count byte reserved       
    for ( i = 0; i < 4; i++ )
    {
        ascii[i] = ascii_frame[i + 5];
    }    
    start = AsciiToTwoByte();
    for ( i = 0; i < 4; i++ )
    {
        ascii[i] = ascii_frame[i + 9];
    }
    
    cant = AsciiToTwoByte();    
    limit = start + cant;
   
    /* querying input registers status */
    for ( i = start; i < limit; i++ )
    {
        status = GetInputRegisterValue(i);
        
        TwoByteToAscii(status);
        ascii_frame[data_count++] = ascii[3];
        ascii_frame[data_count++] = ascii[2];
        ascii_frame[data_count++] = ascii[1];
        ascii_frame[data_count++] = ascii[0];
      
        new_data_count+=2;     // add two bytes to response frame
    }
    
    ByteToAscii(new_data_count);
    ascii_frame[5] = ascii[0];
    ascii_frame[6] = ascii[1];

    gen_lrc();
    
    tx_assci_frame();
    return;
}
void ResponseForceSingleCoil_05() // 0x05, OK!!!!
{
    uint16 coilID = 0;
    uint16 value =  0;    
    uint16 i = 0;            
    for ( i = 0; i < 4; i++ )
    {
        ascii[i] = ascii_frame[i + 5];
    }   
    coilID = AsciiToTwoByte();     //coil index 
    for ( i = 0; i < 4; i++ )
    {
        ascii[i] = ascii_frame[i + 9]; 
    }    
    value = AsciiToTwoByte();       //coil value       
    if ( value == 0 || value == 65280 )     // 0 or 1, 0000h or FF00h
    {    
        SetCoilValue(coilID, value > 0);
    }
    else
    {
        clear_frame();
        return ;    
    }
    
    gen_lrc();

    tx_assci_frame();
    return;
}

void ResponsePresetSingleRegister_06()  //0x06, OK!!!!
{
    uint16 registerID = 0;
    uint16 value =  0;    
    uint16 i = 0;           
    for ( i = 0; i < 4; i++ )
    {
        ascii[i] = ascii_frame[i + 5];
    }   
    registerID = AsciiToTwoByte();     //register index 
    for ( i = 0; i < 4; i++ )
    {
        ascii[i] = ascii_frame[i + 9];
    }    
    value = AsciiToTwoByte();          //register value          
    SetHoldingRegisterValue(registerID, value);
    
    gen_lrc();

    tx_assci_frame();
    return;
}

void ResponseForceMultipleCoils_15() // 0x0F, OK
{
    uint16 start = 0, cant = 0;
    uint16 i = 0, limit = 0;
    uint8 coils = 0, k = 0, new_data_count = 0, tempAdr = 0;
    uint8 index = 0, offset = 0;       
    uint16 idata parameters[20];
    uint8 idata byte_count;    
    for ( i = 0; i < 4; i++ )
    {
        ascii[i] = ascii_frame[i + 5];
    }   
    start = AsciiToTwoByte();    
    for ( i = 0; i < 4; i++ )
    {
        ascii[i] = ascii_frame[i + 9];
    }    
    cant = AsciiToTwoByte();        
    limit = start + cant;
    
    new_data_count = AsciiToByte(ascii_frame[13], ascii_frame[14] );
    data_count = 15;
    
    /* read new coils values */
    byte_count = 0;    
    for ( i = 0; i < new_data_count; i++ )
    {
        coils = AsciiToByte(ascii_frame[data_count], ascii_frame[data_count+1]);
        data_count += 2;        
        parameters[byte_count++] = coils;      
    }

    /* force coils status */
    for ( i = start; i < limit; i++ )
    {        
        tempAdr = i - start;        
        index = tempAdr / 8;
        offset = tempAdr % 8;        
        k = ( parameters[index] & ( 1 << offset ) );    
        SetCoilValue(i, k > 0);    // force status of coil i with k                    
    }
    /* response frame */
    data_count = 13;

    gen_lrc();
    
    tx_assci_frame();
    return;
}

void ResponsePresetMultipleRegisters_16()   // 0x10, OK!!!
{
    uint16 start = 0, cant = 0;
    uint16 i = 0, limit = 0, reg_value = 0;
    signed char j = 0;
    uint8 idata byte_count;          
    for ( i = 0; i < 4; i++ )
    {
        ascii[i] = ascii_frame[i + 5];
    }   
    start = AsciiToTwoByte();        
    for ( i = 0; i < 4; i++ )
    {
        ascii[i] = ascii_frame[i + 9];
    }    
    cant = AsciiToTwoByte();        
    limit = start + cant;
        
    byte_count = AsciiToByte(ascii_frame[13], ascii_frame[14] );
    data_count = 15;
      
    /* read and set new holding registers values */
    byte_count /= 2;
    for ( i = 0; i < byte_count; i++ )
    {       
        for ( j = 0; j < 4; j++ )
        {     
            ascii[j] = ascii_frame[data_count++];        
        }        
        reg_value = AsciiToTwoByte();        
        holding_registers_array[start++] = reg_value;           
    }

    data_count = 13;

    gen_lrc();
    
    tx_assci_frame();
    return;
}
