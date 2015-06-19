/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 05/28/15     1.0_DW0a    Initial coding.
 *                          Add many SD card functions.
 *                          Fix SD card initialization.
 * /******************************************************************************/

/******************************************************************************/
/* Contains Functions for the SD card.
 *
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
#include <stdio.h>         /* For sprintf definition */

#include "user.h"
#include "SD.h"
#include "SPI.h"
#include "MISC.h"

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
unsigned char SD_Initialized = FALSE;
unsigned char Receive_Buffer_Big[SDblockSize];
unsigned char Receive_Buffer_Small[16];
unsigned char R1_Buffer[1];
unsigned char R2_Buffer[2];
unsigned char R3_Buffer[5];
unsigned char R6_Buffer[6];
unsigned char SD_Timeout = 0;
unsigned char SD_CardType = NOT_INITIALIZED;
long SD_CardBlocks = 0;
SDcommand Global_message, *PGlobal_message;

/******************************************************************************/
/* Inline Functions
/******************************************************************************/

/******************************************************************************/
/* Functions
/******************************************************************************/

/******************************************************************************/
/* InitSD
 *
 * The function initializes the SD card pins.
 * MUST BE CALLED AFTER SPI IS INITIALIZED!!!
 *
 * Sequence:
 * 1. send 80 bits with chip select high
 * 2. send cmd0.
 * 3. send cmd8.
 * 4. send special Acmd41
 * 5. Send md 58 to see if it is a high capacity card.
/******************************************************************************/
void InitSD(void)
{
    unsigned char SD_Init_Tries = SD_INIT_MAX;
    unsigned int Acknowledge = FALSE;
    unsigned long arguement;
    long temp;
    cleanBuffer(R1_Buffer, 1);
    cleanBuffer(R2_Buffer, 2);
    cleanBuffer(R3_Buffer, 5);
    cleanBuffer(R6_Buffer, 6);
    Clear_Receive_Buffer_Big();
    Clear_Receive_Buffer_Small();
    SD_CardBlocks = 0;

    PGlobal_message = &Global_message; // pointer to the message
    
    SD_CS_INACTIVE(); /* Deassert the chip select pin */    
    SPI_SD_CSTris = OUTPUT; /* Configure CS pin as an output */

    #ifdef WriteProtect
        // Configure WP pin as an input
        SD_WPTris = INPUT;
    #endif

    #ifdef CardDetect
        // Configure CD pin as an input
        SD_CDTris = INPUT;
    #endif

    if(SDtoSPI())
    {
        /* send CMD8 */
        SD_Timeout = 0;
        while(1)
        {
            if(SD_Timeout >= SD_TIMEOUT_MAX)
            {
                return;
            }
            SD_Timeout++;

            /* send CMD8 */
            Global_message.Transmitter = 1;
            Global_message.Command = CMD8;
            Global_message.Argument = 0x1AA;
            SDaddCRC(PGlobal_message);
            Clear_Receive_Buffer_Small();
            Acknowledge = SD_CMDSPI_send_read(PGlobal_message,R16,Receive_Buffer_Small,YES);
            if(Acknowledge == TRUE)
            {
                if(Receive_Buffer_Small[0] & R1_ILLEGAL_COMMAND)
                {
                    SD_Set_CardType(SD_CARD_TYPE_SD1);
                }
                else
                {
                    if(Receive_Buffer_Small[4] != 0xAA)
                    {
                        /* CMD8 error */
                        return;
                    }
                    SD_Set_CardType(SD_CARD_TYPE_SD2);
                }
                break;
            }
        }

        if(SD_Get_CardType()== SD_CARD_TYPE_SD2)
        {
            arguement = 0x40000000;
        }
        else
        {
            arguement = 0;
        }

        while(SD_Init_Tries > 0)
        {
            /* send ACMD41 */
            SD_Timeout = 0;
            while(1)
            {
                if(SD_Timeout >= SD_TIMEOUT_MAX)
                {
                    return;
                }
                SD_Timeout++;

                /* send ACMD41 */
                Global_message.Transmitter = 1;
                Global_message.Command = ACMD41;
                Global_message.Argument = arguement;
                SDaddCRC(PGlobal_message);
                Acknowledge = SD_ACMDSPI_send_read(PGlobal_message,R1,R1_Buffer);
                if(Acknowledge == TRUE)
                {
                    if(R1_Buffer[0] == R1_GOOD_NOT_IDLE)
                    {
                        SD_Initialized = TRUE;
                        break;
                    }
                }
            }

            /* check for SDHC card */
            if(SD_Get_CardType()==SD_CARD_TYPE_SD2)
            {
                /* send CMD58 */
                Global_message.Transmitter = 1;
                Global_message.Command = CMD58;
                Global_message.Argument = 0;
                SDaddCRC(PGlobal_message);
                Clear_Receive_Buffer_Small();
                Acknowledge = SD_CMDSPI_send_read(PGlobal_message,R16,Receive_Buffer_Small,YES);
                if(Acknowledge == TRUE)
                {
                    if((Receive_Buffer_Small[1] & 0XC0) == 0XC0)
                    {
                        SD_Set_CardType(SD_CARD_TYPE_SDHC);
                        break;
                    }
                }
            }

            /* Set block length. For SDHC Card, the block length is fixed at 512 */
            if(SD_Get_CardType()!=SD_CARD_TYPE_SDHC)
            {
                /* send CMD58 */
                Global_message.Transmitter = 1;
                Global_message.Command = CMD16;
                Global_message.Argument = 512;
                SDaddCRC(PGlobal_message);
                Clear_Receive_Buffer_Small();
                Acknowledge = SD_CMDSPI_send_read(PGlobal_message,R1,R1_Buffer,YES);
                if(Acknowledge == TRUE)
                {
                    if((Receive_Buffer_Small[1] & 0XC0) == 0XC0)
                    {
                        SD_Set_CardType(SD_CARD_TYPE_SDHC);
                        break;
                    }
                }
            }

            SD_Init_Tries--;
        }
        SetSPISpeed(400.0); /* set speed to 4MHz */
        SPI_Enable();
        delayUS(1000);/* needed for SPI clock to stabalize */
    }
    temp = Total_Num_Blocks();
    if( temp >0 )
    {
        SD_CardBlocks = temp;
    }
}

/******************************************************************************/
/* SD_Set_CardType
 *
 * The function sets the SD card type.
/******************************************************************************/
void SD_Set_CardType(unsigned char type)
{
    SD_CardType = type;
}

/******************************************************************************/
/* SD_Get_CardType
 *
 * The function gets the SD card type.
/******************************************************************************/
unsigned char SD_Get_CardType(void)
{
    return SD_CardType;
}

/******************************************************************************/
/* Clear_Receive_Buffer_Big
 *
 * The function clears the large receive buffer.
/******************************************************************************/
void Clear_Receive_Buffer_Big(void)
{
    cleanBuffer(Receive_Buffer_Big, SDblockSize);
}

/******************************************************************************/
/* Clear_Receive_Buffer_Small
 *
 * The function clears the small receive buffer.
/******************************************************************************/
void Clear_Receive_Buffer_Small(void)
{
    cleanBuffer(Receive_Buffer_Small, R16);
}

/******************************************************************************/
/* Total_Num_Blocks
 *
 * The function returns the total number of blocks on the SD memory card.
/******************************************************************************/
long Total_Num_Blocks(void)
{
    long C_Size = 0;
    long C_Mult = 0;

    Global_message.Transmitter = 1;
    Global_message.Command = CMD9;
    Global_message.Argument = 0;
    SDaddCRC(PGlobal_message);
    if(!SD_readRegister(PGlobal_message))
    {
        return -1;
    }

    // compute size
    C_Size = ((Receive_Buffer_Small[0x08] & 0xC0) >> 6) | ((Receive_Buffer_Small[0x07] & 0xFF) << 2) | ((Receive_Buffer_Small[0x06] & 0x03) << 10);
    C_Mult = ((Receive_Buffer_Small[0x08] & 0x80) >> 7) | ((Receive_Buffer_Small[0x08] & 0x03) << 2);
    return ((C_Size+1) << (C_Mult+2));
}

/******************************************************************************/
/* SD_readRegister
 *
 * Reads an SD register and stores it in Receive_Buffer_Small.
/******************************************************************************/
unsigned char SD_readRegister(SDcommand* pmessage)
{
    unsigned char i;
    unsigned char* Pbuf = Receive_Buffer_Small;

    R1_Buffer[0] = 0xFF;

    SD_CMDSPI_send_read(pmessage,R1,R1_Buffer, NO);
    while(R1_Buffer[0] != 0)
    {
        SD_Timeout = 0;
        while(!SPIwrite_read(0xFF,R1_Buffer))
        {
            SD_Timeout++;
            if(SD_Timeout >= SD_TIMEOUT_MAX)
            {
                delayUS(10);
                SD_CS_INACTIVE();
                delayUS(10);
                return FAIL;
            }
        }
    }

     // wait for data token
    R1_Buffer[0] = 0xFF;
    SD_Timeout = 0;
    while(R1_Buffer[0] != DATA_START_BLOCK)
    {
        while(!SPIwrite_read(0xFF,R1_Buffer))
        {
            SD_Timeout++;
            if(SD_Timeout >= SD_TIMEOUT_MAX)
            {
                delayUS(10);
                SD_CS_INACTIVE();
                delayUS(10);
                return FAIL;
            }
        }
    }
    // read data
    for (i=0; i<16; i++)
    {
        SPIwrite_read(0xFF,Pbuf);
        Pbuf++;
    }
    // read CRC
    SPIwrite(0xFF); // LSB
    SPIwrite(0xFF); // MSB

    SD_Clear();
    delayUS(10);
    SD_CS_INACTIVE();
    delayUS(10);
    return PASS;
}

/******************************************************************************/
/* SD_readBlock
 *
 * Reads an SD block and stores it in Receive_Buffer_Big.
 * Command Argument:
 *
 * SDHC and SDXC use the 32-bit argument of memory access commands as block
 * address format. Block length is fixed to 512 bytes regardless CMD16,
 * format. Block SDSC uses the 32-bit argument of memory access commands as
 * byte address length is determined by CMD16,
 * i.e.:
 * (a) Argument 0001h is byte address 0001h in the SDSC and 0001h block in SDHC
 *     and SDXC
 * (b) Argument 0200h is byte address 0200h in the SDSC and 0200h block in SDHC
 *     and SDXC
/******************************************************************************/
unsigned char SD_readBlock(long blockIndex)
{
    unsigned int i;
    unsigned char* Pbuf = Receive_Buffer_Big;
    unsigned long block;
    unsigned int BytesRead;

    Global_message.Transmitter = 1;
    Global_message.Command = CMD17;
    block = blockIndex;
    R1_Buffer[0] = 0xFF;

    if(SD_Get_CardType() != SD_CARD_TYPE_SDHC)
    {
        block <<= 9;
    }
    Global_message.Argument = block;
    SDaddCRC(PGlobal_message);

    SD_CMDSPI_send_read(PGlobal_message,R1,R1_Buffer,NO);
    SD_Timeout = 0;
    while(R1_Buffer[0] != 0)
    {        
        while(!SPIwrite_read(0xFF,R1_Buffer))
        {
            SD_Timeout++;
            if(SD_Timeout >= SD_TIMEOUT_MAX)
            {
                delayUS(10);
                SD_CS_INACTIVE();
                delayUS(10);
                return FAIL;
            }
        }
    }

     // wait for data token
    SD_Timeout = 0;
    R1_Buffer[0] = 0xFF;
    while(R1_Buffer[0] != DATA_START_BLOCK)
    {
        while(!SPIwrite_read(0xFF,R1_Buffer))
        {
            SD_Timeout++;
            if(SD_Timeout >= SD_TIMEOUT_MAX)
            {
                delayUS(10);
                SD_CS_INACTIVE();
                delayUS(10);
                return FAIL;
            }
        }
    }

    // read data
    BytesRead = 0;
    for (i=0; i<SDblockSize; i++)
    {
        if(SPIwrite_read(0xFF,Pbuf))
        {
            BytesRead++;
        }
        Pbuf++;
    }
    // read CRC
    SPIwrite(0xFF); // LSB
    SPIwrite(0xFF); // MSB

    SD_Clear();
    delayUS(10);
    SD_CS_INACTIVE();
    delayUS(10);
    if(BytesRead == SDblockSize)
    {
        return PASS;
    }
    return FAIL;
}

/******************************************************************************/
/* TestCRC
 *
 * The function initializes the CRC algorithm.
/******************************************************************************/
void TestCRC(void)
{
    unsigned char result =0;

    result = CRC7_40bits(0x40,0x00000000,CRCMASK_7);
    Nop();
}

/******************************************************************************/
/* SDintoSPI
 *
 * The function attemps to put the SD card into SPI mode. The function return
 *  TRUE if the SD card is in SPI mode and FALSE if it wont go into this mode.
/******************************************************************************/
unsigned char SDtoSPI(void)
{
    SDcommand message, *Pmessage;
    unsigned char i;
    unsigned int Acknowledge = FALSE;

    Pmessage = &message;


    message.Transmitter = 1;
    message.Command = CMD0;
    message.Argument = 0;
    SDaddCRC(Pmessage);

    SetSPISpeed(400.0); /* set speed to 400kHz */
    SPI_Enable();
    delayUS(1000);/* needed for SPI clock to stabalize */

    for(i=0;i<SD_INIT_MAX;i++)
    {        
        SD_RESET();

        delayUS(1000);
        Acknowledge = SD_CMDSPI_send_read(Pmessage,R1,R1_Buffer,YES);
        if(Acknowledge == TRUE)
        {
            if(R1_Buffer[0] == R1_IDLE_STATE)
            {
                return TRUE;
            }
        }
        delayUS(200);
    }
    return FALSE;
}

/******************************************************************************/
/* SD_CMDSPI_send
 *
 * The function sends an SD card command.
/******************************************************************************/
void SD_CMDSPI_send(SDcommand* message)
{
    unsigned char first,second,third,forth,fifth,sixth;

    first = message->Transmitter;
    first <<= 6;
    first |= message->Command;
    second = (unsigned char)    ((message->Argument & 0xFF000000) >> 24);
    third = (unsigned char)     ((message->Argument & 0x00FF0000) >> 16);
    forth = (unsigned char)     ((message->Argument & 0x0000FF00) >> 8);
    fifth = (unsigned char)      (message->Argument & 0x000000FF);
    sixth = (message->CRC << 1) + 1;

    SD_CS_ACTIVE();
    delayUS(10);
    SD_Clear();
    delayUS(10);
    SPIwrite(first);
    SPIwrite(second);
    SPIwrite(third);
    SPIwrite(forth);
    SPIwrite(fifth);
    SPIwrite(sixth);
}

/******************************************************************************/
/* SD_ACMDSPI_send_read
 *
 * The function sends an SD card Application specific command and listens for
 * a response.
/******************************************************************************/
unsigned char SD_ACMDSPI_send_read(SDcommand* pmessage, unsigned int ResponseType, unsigned char* read)
{
    SDcommand temp_message, *temp_Pmessage;
    unsigned char Acknowledge;

    temp_Pmessage = &temp_message; // pointer to the message
    
    temp_message.Transmitter = 1;
    temp_message.Command = CMD55;
    temp_message.Argument = 0;
    SDaddCRC(temp_Pmessage);

    SD_CMDSPI_send(temp_Pmessage);
    SD_WaitResponse();
    Acknowledge = SD_CMDSPI_send_read(pmessage,ResponseType,read,YES);
    return (Acknowledge);
}
/******************************************************************************/
/* SD_CMDSPI_send_read
 *
 * The function sends an SD card command and listens for a response.
/******************************************************************************/
unsigned char SD_CMDSPI_send_read(SDcommand* message, unsigned int ResponseType, unsigned char* read, unsigned char CSonGood)
{
    unsigned response = FALSE;
    unsigned int i;

    SD_CMDSPI_send(message);

    SD_Timeout = 0;
    if(ResponseType == R1)
    {
        while(!SPIwrite_read(0xFF,read))
        {
            SD_Timeout++;
            if(SD_Timeout >= SD_TIMEOUT_MAX)
            {
                delayUS(10);
                SD_CS_INACTIVE();
                delayUS(10);
                return FAIL;
            }
            delayUS(10);
        }
        response = TRUE;
    }
    else
    {
        for(i=0;i<ResponseType;i++)
        {
            while(!SPIwrite_read(0xFF,read))
            {
                SD_Timeout++;
                if(SD_Timeout >= SD_TIMEOUT_MAX)
                {
                    delayUS(10);
                    SD_CS_INACTIVE();
                    if(response)
                    {
                        return TRUE;
                    }
                    return FALSE;
                }
                delayUS(10);
            }
            response = TRUE;
            read++;
        }
    }

    if(CSonGood)
    {
        delayUS(10);
        SD_CS_INACTIVE();
    }
    if(response)
    {
        return TRUE;
    }
    return FALSE;
}

/******************************************************************************/
/* SDaddCRC
 *
 * The function attaches the messages CRC.
/******************************************************************************/
void SDaddCRC(SDcommand* message)
{
    unsigned long MSBmessage, LSBmessage;

    MSBmessage = message->Transmitter;
    MSBmessage <<= 6;
    MSBmessage |= message->Command;
    LSBmessage = message->Argument;

    message->CRC = CRC7_40bits(MSBmessage, LSBmessage, CRCMASK_7);
}

/******************************************************************************/
/* CRC40
 *
 * The function calculates a 40 bit CRC which is used for SD card
 *  communication.
/******************************************************************************/
unsigned char CRC7_40bits(unsigned long MSBmessage, unsigned long LSBmessage, unsigned char mask)
{
    unsigned long First,Second;
    unsigned char data = 0;
    char i;
    unsigned long temp;
    
    First  = MSBmessage; /* bit  32 - 39 */
    Second = LSBmessage; /* bit  0 - 31  */

    data = First;
    
    for (i=31; i>=0; i--)
    {
        if(data & 0x80)
        {
            data ^=  mask;
        }
        data <<= 1;
        temp = 1;
        temp <<= i;
        temp &= Second;
        temp >>= i;
        data |= (unsigned char) temp;
    }
    if(!data)
    {
        return 0;
    }

    for (i=8;i > 0;i--)
    {
        if(data & 0x80)
        {
            data ^=  mask;
        }
        if(i != 1)
        {
            data <<= 1;
        }
    }
    return (data);
}

/******************************************************************************/
/* SD_RESET
 *
 * The function resets the SD card by sending clock pulses with cs high.
/******************************************************************************/
void SD_RESET(void)
{
    /* Write 12 bytes aka 96 cycles */
    SD_CS_INACTIVE();
    delayUS(10);
    SPIwrite(0xFF);
    SPIwrite(0xFF);
    SPIwrite(0xFF);
    SPIwrite(0xFF);
    SPIwrite(0xFF);
    SPIwrite(0xFF);
    SPIwrite(0xFF);
    SPIwrite(0xFF);
    SPIwrite(0xFF);
    SPIwrite(0xFF);
    SPIwrite(0xFF);
    SPIwrite(0xFF);
}

/******************************************************************************/
/* SD_Clear
 *
 * The function waits until the SD card is done talking.
/******************************************************************************/
void SD_Clear(void)
{
    /* Write data until there is no response */
    while(SPIwrite_read(0xFF, NULL));
}

/******************************************************************************/
/* SD_Clear
 *
 * The function waits until the SD card talks.
/******************************************************************************/
unsigned char SD_WaitResponse(void)
{
    SD_Timeout = 0;
    while(!SPIwrite_read(0xFF, NULL))
    {
        SD_Timeout++;
        if(SD_Timeout >= SD_TIMEOUT_MAX)
        {
            return FALSE;
        }
    }
    return TRUE;
}

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/