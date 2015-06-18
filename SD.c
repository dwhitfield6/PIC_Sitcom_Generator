/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 05/28/15     1.0_DW0a    Initial coding.
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
/******************************************************************************/
void InitSD(void)
{
    unsigned char SD_Init_Tries = SD_INIT_MAX;
    unsigned int Acknowledge = FALSE;
    cleanBuffer(R1_Buffer, 1);
    cleanBuffer(R2_Buffer, 2);
    cleanBuffer(R3_Buffer, 5);
    cleanBuffer(R6_Buffer, 6);
    Clear_Receive_Buffer();

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
            Acknowledge = SD_CMDSPI_send_read(PGlobal_message,R1,R1_Buffer);
            if(Acknowledge == TRUE)
            {
                break;
            }
        }

        while(SD_Init_Tries > 0)
        {
            /* send CMD55 */
            SD_Timeout = 0;
            while(1)
            {
                if(SD_Timeout >= SD_TIMEOUT_MAX)
                {
                    return;
                }
                SD_Timeout++;

                /* send CMD55 */
                Global_message.Transmitter = 1;
                Global_message.Command = CMD55;
                Global_message.Argument = 0;
                SDaddCRC(PGlobal_message);
                Acknowledge = SD_CMDSPI_send_read(PGlobal_message,R1,R1_Buffer);
                if(Acknowledge == TRUE)
                {
                    break;
                }
            }

            /* send CMD41 */
            SD_Timeout = 0;
            while(1)
            {
                if(SD_Timeout >= SD_TIMEOUT_MAX)
                {
                    return;
                }
                SD_Timeout++;

                /* send CMD41 */
                Global_message.Transmitter = 1;
                Global_message.Command = CMD41;
                Global_message.Argument = 0;
                SDaddCRC(PGlobal_message);
                Acknowledge = SD_CMDSPI_send_read(PGlobal_message,R1,R1_Buffer);
                if(Acknowledge == TRUE)
                {
                    if(R1_Buffer[0] == R1_GOOD_NOT_IDLE)
                    {
                                       /* send CMD41 */
                        Global_message.Transmitter = 1;
                        Global_message.Command = CMD55;
                        Global_message.Argument = 0;
                        SDaddCRC(PGlobal_message);
                        SD_CMDSPI_send_read(PGlobal_message,R1,R1_Buffer);
                        SetSPISpeed(4000.0); /* set speed to 4MHz */
                        SPI_Enable();
                        delayUS(1000);/* needed for SPI clock to stabalize */
                        SD_Initialized = TRUE;
                        return;
                    }
                    else
                    {
                        break;
                    }
                }
            }
            SD_Init_Tries--;
        }
    }
    SD_readBlock(0);
}

/******************************************************************************/
/* Clear_Receive_Buffer
 *
 * The function clears the receive buffer.
/******************************************************************************/
void Clear_Receive_Buffer(void)
{
    cleanBuffer(Receive_Buffer_Big, SDblockSize);
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

    SD_CMDSPI_send_read(pmessage,R1,R1_Buffer);
    while(R1_Buffer[0] != 0)
    {
        SD_Timeout = 0;
        while(!SPIwrite_read(0xFF,R1_Buffer))
        {
            SD_Timeout++;
            if(SD_Timeout >= SD_TIMEOUT_MAX)
            {
                SD_CS_INACTIVE();
                return FAIL;
            }
        }
    }

     // wait for data token
    R1_Buffer[0] = 0xFF;
    while(R1_Buffer[0] != 0xFE)
    {
        SD_Timeout = 0;
        while(!SPIwrite_read(0xFF,R1_Buffer))
        {
            SD_Timeout++;
            if(SD_Timeout >= SD_TIMEOUT_MAX)
            {
                SD_CS_INACTIVE();
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
    SD_CS_INACTIVE();
    return PASS;
}

/******************************************************************************/
/* SD_readBlock
 *
 * Reads an SD block and stores it in Receive_Buffer_Small.
/******************************************************************************/
unsigned char SD_readBlock(long blockIndex)
{
    unsigned int i;
    unsigned char* Pbuf = Receive_Buffer_Small;

    Global_message.Transmitter = 1;
    Global_message.Command = CMD17;
    Global_message.Argument = blockIndex * SDblockSize;
    SDaddCRC(PGlobal_message);
    R1_Buffer[0] = 0xFF;

    SD_CMDSPI_send_read(PGlobal_message,R1,R1_Buffer);
    while(R1_Buffer[0] != 0)
    {
        SD_Timeout = 0;
        while(!SPIwrite_read(0xFF,R1_Buffer))
        {
            SD_Timeout++;
            if(SD_Timeout >= SD_TIMEOUT_MAX)
            {
                SD_CS_INACTIVE();
                return FAIL;
            }
        }
    }

     // wait for data token
    R1_Buffer[0] = 0xFF;
    while(R1_Buffer[0] != 0xFE)
    {
        SD_Timeout = 0;
        while(!SPIwrite_read(0xFF,R1_Buffer))
        {
            SD_Timeout++;
            if(SD_Timeout >= SD_TIMEOUT_MAX)
            {
                SD_CS_INACTIVE();
                return FAIL;
            }
        }
    }

    // read data
    for (i=0; i<SDblockSize; i++)
    {
        SPIwrite_read(0xFF,Pbuf);
        Pbuf++;
    }
    // read CRC
    SPIwrite(0xFF); // LSB
    SPIwrite(0xFF); // MSB
    SD_CS_INACTIVE();
    return PASS;
}

/******************************************************************************/
/* TestCRC
 *
 * The function initializes the CRC module.
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

    SetSPISpeed(200.0); /* set speed to 200kHz */
    SPI_Enable();
    delayUS(1000);/* needed for SPI clock to stabalize */

    for(i=0;i<SD_INIT_MAX;i++)
    {        
        SD_RESET();

        delayUS(1000);
        Acknowledge = SD_CMDSPI_send_read(Pmessage,R1,R1_Buffer);
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
 * The function attemps to put the SD card into SPI mode. The function return
 *  TRUE if the SD card is in SPI mode and FALSE if it wont go into this mode.
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
    SPIwrite(first);
    SPIwrite(second);
    SPIwrite(third);
    SPIwrite(forth);
    SPIwrite(fifth);
    SPIwrite(sixth);
}

/******************************************************************************/
/* SD_CMDSPI_send_read
 *
 * The function attemps to put the SD card into SPI mode. The function return
 *  TRUE if the SD card is in SPI mode and FALSE if it wont go into this mode.
/******************************************************************************/
unsigned char SD_CMDSPI_send_read(SDcommand* message, unsigned char ResponseType, unsigned char* read)
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
                SD_CS_INACTIVE();
                return FAIL;
            }
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
                    SD_CS_INACTIVE();
                    return FAIL;
                }
            }
            read++;
        }
        response = TRUE;
    }

    SD_CS_INACTIVE();
    if(response)
    {
        return TRUE;
    }
    return FALSE;
}

/******************************************************************************/
/* SDintoSPI
 *
 * The function attemps to put the SD card into SPI mode. The function return
 *  TRUE if the SD card is in SPI mode and FALSE if it wont go into this mode.
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

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/