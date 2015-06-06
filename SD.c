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
    
    SD_CS_INACTIVE(); /* Deassert the chip select pin */    
    SPI_SD_CSTris = OUTPUT; /* Configure CS pin as an output */
    TestCRC();
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
        SD_Initialized = TRUE;
    }
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
    unsigned int response;

    Pmessage = &message;


    message.Transmitter = 1;
    message.Command = CMD0;
    message.Argument = 0;
    SDaddCRC(Pmessage);

    SetSPISpeed(200.0); /* set speed to 400kHz */
    SPI_Enable();
    delayUS(1000);/* needed for SPI clock to stabalize */

    //for(i=0;i<SD_INIT_MAXATTEMPTS;i++)
    while(1)
    {
        /* Write 12 bytes aka 96 cycles */
        SD_CS_ACTIVE();
        SPIwrite(0xFFFF);
        SPIwrite(0xFFFF);
        SPIwrite(0xFFFF);
        SPIwrite(0xFFFF);
        SPIwrite(0xFFFF);
        SPIwrite(0xFFFF);
        SD_CS_INACTIVE();
        Cmd0:
        delayUS(2000);
        SD_CS_INACTIVE();
        delayUS(100);
        SD_CS_ACTIVE();

        Acknowledge = SDsendCMDSPI(Pmessage,20,&response);
        SD_CS_INACTIVE();
        if(Acknowledge == TRUE)
        {
            if(response == 1)
            {
                Nop();
                return TRUE;
            }
            else
            {
                //goto Cmd0;
            }
        }
        delayUS(2000);
    }
    return FALSE;
}

/******************************************************************************/
/* SDintoSPI
 *
 * The function attemps to put the SD card into SPI mode. The function return
 *  TRUE if the SD card is in SPI mode and FALSE if it wont go into this mode.
/******************************************************************************/
unsigned char SDsendCMDSPI(SDcommand* message, unsigned char ReadCycles, unsigned int* read)
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

    SPIwrite((first << 8) + second);
    SPIwrite((third << 8) + forth);
    SPIwrite((fifth << 8) + sixth);
    SD_CS_INACTIVE();
    delayUS(100);
    SD_CS_ACTIVE();
    if(SPIwrite_read(0xFFFF,read))
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

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/