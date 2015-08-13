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

#include "USER.h"
#include "SD.h"
#include "SPI.h"
#include "MISC.h"

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
volatile unsigned char SD_State = NOT_INITIALIZED;
unsigned char SD_Receive_Buffer_Big[SDblockSize];
unsigned char SD_Receive_Buffer_Small[20];
unsigned char R1_Buffer[1];
unsigned char R2_Buffer[2];
unsigned char R3_Buffer[5];
unsigned char R6_Buffer[6];
unsigned long SD_Timeout = 0;
unsigned char SD_CardType = NOT_INITIALIZED;
SDcommand Global_message;
SDcommand* PGlobal_message;
SDproperties SD;

/******************************************************************************/
/* Inline Functions
/******************************************************************************/

/******************************************************************************/
/* SD_POWER
 *
 * The function Asserts the chip select pin.
/******************************************************************************/
inline void SD_POWER(unsigned char State)
{
#ifndef PROTOBOARD
    if(State)
    {
        LATC |= SD_Power;
    }
    else
    {
        LATC &= ~SD_Power;
    }
#else
    Nop();
#endif
}

/******************************************************************************/
/* SD_CS_INACTIVE
 *
 * The function Asserts the chip select pin.
/******************************************************************************/
inline void SD_CS_ACTIVE(void)
{
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    LATB &= ~SPI_SD_CS;
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
}

/******************************************************************************/
/* SD_CS_ACTIVE
 *
 * The function Asserts the chip select pin.
/******************************************************************************/
inline void SD_CS_INACTIVE(void)
{
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    LATB |= SPI_SD_CS;
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
}

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
    MSC_CleanBuffer(R1_Buffer, 1);
    MSC_CleanBuffer(R2_Buffer, 2);
    MSC_CleanBuffer(R3_Buffer, 5);
    MSC_CleanBuffer(R6_Buffer, 6);
    SD_Clear_SD_Receive_Buffer_Big();
    SD_Clear_SD_Receive_Buffer_Small();
    SD_DeleteCard();

    PGlobal_message = &Global_message; // pointer to the message
    
    SD_CS_INACTIVE(); /* Deassert the chip select pin */    
    SPI_SD_CSTris = OUTPUT; /* Configure CS pin as an output */

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
            SD_Clear_SD_Receive_Buffer_Small();
            Acknowledge = SD_CMDSPI_send_read(PGlobal_message,R16,SD_Receive_Buffer_Small,YES);
            if(Acknowledge == TRUE)
            {
                if(SD_Receive_Buffer_Small[0] & R1_ILLEGAL_COMMAND)
                {
                    SD_Set_CardType(SD_CARD_TYPE_SD1);
                }
                else
                {
                    if(SD_Receive_Buffer_Small[4] != 0xAA)
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
                SD_Clear_SD_Receive_Buffer_Small();
                Acknowledge = SD_CMDSPI_send_read(PGlobal_message,R16,SD_Receive_Buffer_Small,YES);
                if(Acknowledge == TRUE)
                {
                    if((SD_Receive_Buffer_Small[1] & 0XC0) == 0XC0)
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
                SD_Clear_SD_Receive_Buffer_Small();
                Acknowledge = SD_CMDSPI_send_read(PGlobal_message,R1,R1_Buffer,YES);
                if(Acknowledge == TRUE)
                {
                    if((SD_Receive_Buffer_Small[1] & 0XC0) == 0XC0)
                    {
                        SD_Set_CardType(SD_CARD_TYPE_SDHC);
                        break;
                    }
                }
            }

            SD_Init_Tries--;
        }
        SPI_SetSpeed(SPI_FAST); /* set speed to 4MHz */
        SPI_Enable();
        MSC_DelayUS(1000);/* needed for SPI clock to stabalize */
        if(SD_Properties())
        {
            SD_State = INITIALIZED;
        }
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
/* SD_Clear_SD_Receive_Buffer_Big
 *
 * The function clears the large receive buffer.
/******************************************************************************/
void SD_Clear_SD_Receive_Buffer_Big(void)
{
    MSC_CleanBuffer(SD_Receive_Buffer_Big, SDblockSize);
}

/******************************************************************************/
/* SD_Clear_SD_Receive_Buffer_Small
 *
 * The function clears the small receive buffer.
/******************************************************************************/
void SD_Clear_SD_Receive_Buffer_Small(void)
{
    MSC_CleanBuffer(SD_Receive_Buffer_Small, R16);
}

/******************************************************************************/
/* SD_DeleteCard
 *
 * The function clears the SD card properties.
/******************************************************************************/
void SD_DeleteCard(void)
{
    SD.CSDVer = 0;
    SD.Size = 0;
    SD.RawSize = 0;
    SD.ReadLength = 0;
    SD.WriteLength = 0;
    SD.Speed = 0;
    SD.CCC = 0;
    SD.PERM_WP = 0;
    SD.WP = 0;
    SD.Blocks = 0;
}

/******************************************************************************/
/* SD_Properties
 *
 * The function reads the SD card for its properties.
/******************************************************************************/
unsigned char SD_Properties(void)
{
    unsigned int C_Mult, C_Size_Mult;
    unsigned long C_Size,temp;

    Global_message.Transmitter = 1;
    Global_message.Command = CMD9;
    Global_message.Argument = 0;
    SDaddCRC(PGlobal_message);

    /* read the CSD register */

    if(!SD_readRegister(PGlobal_message))
    {
        return FAIL;
    }

    SD.CSDVer = ((SD_Receive_Buffer_Small[0] & 0b11000000) >> 6) + 1;

    /* TRAN_SPEED */
    if(SD_Receive_Buffer_Small[3] == 0x32 )
    {
        SD.Speed = 25;
    }
    else if(SD_Receive_Buffer_Small[3] == 0x5A )
    {
        SD.Speed = 50;
    }
    else if(SD_Receive_Buffer_Small[3] == 0x0B )
    {
        SD.Speed = 100;
    }
    else // 2Bh
    {
        SD.Speed = 200;
    }

    /* Card Command Class */
    SD.CCC          = ((unsigned int)SD_Receive_Buffer_Small[4] << 4) + (((unsigned int) SD_Receive_Buffer_Small[5] & 0b11110000) >> 4);

    SD.ReadLength   = ( 0x1 << (SD_Receive_Buffer_Small[5] &0b00001111));

    if(SD.CSDVer == 2)
    {
        temp = SD_Receive_Buffer_Small[7];
        C_Size      = ((unsigned long)(temp & 0b00011111) << 17) + (SD_Receive_Buffer_Small[8] << 8) + (SD_Receive_Buffer_Small[9]);
        SD.RawSize     = (C_Size + 1) * SDblockSize; // in Kbytes
        SD.Blocks   =  (SD.RawSize >> 9) * 1000;
    }
    else
    {
        C_Size_Mult = ((SD_Receive_Buffer_Small[10] & 0b100000000) >> 7) + ((SD_Receive_Buffer_Small[9] & 0b000000011) << 1);
        C_Mult      = (2 << (C_Size_Mult + 2)) * (C_Size_Mult << 8);
        C_Size      = ((SD_Receive_Buffer_Small[6] & 0b110000000) >> 6) + (SD_Receive_Buffer_Small[7] << 2) + ((SD_Receive_Buffer_Small[8] & 0b000000011) << 10);
        SD.Blocks = ((C_Size + 1) * C_Mult);
        SD.RawSize = SD.Blocks * SDblockSize;        
    }

    if(SD.RawSize < 1000000)
    {
        SD.Size = MSC_LG_Round(SD.RawSize,5);
    }
    else
    {
        SD.Size = MSC_LG_Round(SD.RawSize,6);
    }

    SD.WP       = (SD_Receive_Buffer_Small[14] & 0b00010000) >> 4;
    SD.PERM_WP  = (SD_Receive_Buffer_Small[14] & 0b00100000) >> 5;
    SD.WriteLength = (0x1 << (((SD_Receive_Buffer_Small[12] & 0b00000011) << 2) + ((SD_Receive_Buffer_Small[13] & 0b11000000) >> 6)));
    return TRUE;
}

/******************************************************************************/
/* SD_readStatus
 *
 * Reads a the SD status Register.
/******************************************************************************/
unsigned char SD_readStatus(void)
{
    SD_SetCMD(CMD13, 0);
    if(SD_CMDSPI_send_read(PGlobal_message,R2,R2_Buffer, YES))
    {
        return TRUE;
    }
    return FALSE;
}
/******************************************************************************/
/* SD_readRegister
 *
 * Reads an SD register and stores it in SD_Receive_Buffer_Small.
/******************************************************************************/
unsigned char SD_readRegister(SDcommand* pmessage)
{
    unsigned char i;
    unsigned char* Pbuf = SD_Receive_Buffer_Small;
    unsigned char MISOCheck;

    R1_Buffer[0] = 0xFF;
    SD_Clear_SD_Receive_Buffer_Small();
    
    SD_CMDSPI_send_read(pmessage,R1,R1_Buffer, NO);
    while(R1_Buffer[0] != 0)
    {
        SD_Timeout = 0;
        while(!SPI_WriteRead(0xFF,R1_Buffer,YES))
        {
            if(SD_CardPresent() == FAIL)
            {
                return FAIL;
            }
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
    SD_Timeout = 0;
    while(R1_Buffer[0] != DATA_START_BLOCK)
    {
        while(!SPI_WriteRead(0xFF,R1_Buffer,YES))
        {
            if(SD_CardPresent() == FAIL)
            {
                return FAIL;
            }
            SD_Timeout++;
            if(SD_Timeout >= SD_TIMEOUT_MAX)
            {
                SD_CS_INACTIVE();
                return FAIL;
            }
        }
    }
    // read data
    MISOCheck = YES;
    for (i=0; i<16; i++)
    {
        SPI_WriteRead(0xFF,Pbuf,MISOCheck);
        MISOCheck = NO;
        Pbuf++;
    }
    // read CRC
    SPIwrite(0xFF); // LSB
    SPIwrite(0xFF); // MSB

    SD_Clear();
    SD_CS_INACTIVE();
    return PASS;
}

/******************************************************************************/
/* SD_readBlock
 *
 * Reads an SD block and stores it in SD_Receive_Buffer_Big.
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
unsigned char SD_readBlock(unsigned long blockIndex)
{
    unsigned int i;
    unsigned char* Pbuf = SD_Receive_Buffer_Big;
    unsigned long block;
    unsigned int BytesRead;
    unsigned char MISOCheck;
    unsigned long SDRead_Timer=0;

    Global_message.Transmitter = 1;
    Global_message.Command = CMD17;
    block = blockIndex;
    R1_Buffer[0] = 0xFF;
    SD_Clear_SD_Receive_Buffer_Big();
    
    if(SD_Get_CardType() != SD_CARD_TYPE_SDHC)
    {
        block <<= 9;
    }
    Global_message.Argument = block;

    SD_CMDSPI_send_read(PGlobal_message,R1,R1_Buffer,NO);
    
    SD_Timeout = 0;
    while(R1_Buffer[0] != 0)
    {       
        while(!SPI_WriteRead(0xFF,R1_Buffer,YES))
        {
            if(SD_CardPresent() == FAIL)
            {
                Nop();
                return FAIL;
            }
            SD_Timeout++;
            if(SD_Timeout >= SD_TIMEOUT_MAX)
            {
                SD_CS_INACTIVE();
                return FAIL;
            }
        }
    }

     // wait for data token
    SD_Timeout = 0;
    SDRead_Timer=0;
    R1_Buffer[0] = 0xFF;
    while(R1_Buffer[0] != DATA_START_BLOCK)
    {
        SDRead_Timer++;
        while(!SPI_WriteRead(0xFF,R1_Buffer,YES))
        {
            if(SD_CardPresent() == FAIL)
            {
                Nop();
                return FAIL;
            }
            SD_Timeout++;
            if(SD_Timeout >= SD_TIMEOUT_MAX)
            {
                SD_CS_INACTIVE();
                return FAIL;
            }
        }
    }

    // read data
    BytesRead = 0;
    MISOCheck = YES;
    for (i=0; i<SDblockSize; i++)
    {
        if(SPI_WriteRead(0xFF,Pbuf,MISOCheck))
        {
            BytesRead++;
        }
        MISOCheck = NO;
        Pbuf++;
    }
    // read CRC
    SPIwrite(0xFF); // LSB
    SPIwrite(0xFF); // MSB

    SD_Clear();
    SD_CS_INACTIVE();
    if(BytesRead >= (SDblockSize - 2))
    {
        return PASS;
    }
    return FAIL;
}

/******************************************************************************/
/* SD_readMultiple
 *
 * Reads SD blocks and stores it in SD_Receive_Buffer_Big.
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
unsigned char SD_readMultipleBlock(long StartIndex, long StopIndex, void (*fpointer)(void))
{
    unsigned int i;
    unsigned char* Pbuf = SD_Receive_Buffer_Big;
    unsigned long block;
    unsigned long BytesRead;
    unsigned long BytesReadTotal;
    unsigned long j;
    unsigned char MISOCheck;

    if(StartIndex > StopIndex)
    {
        return FAIL;
    }
    Global_message.Transmitter = 1;
    Global_message.Command = CMD18;
    block = StartIndex;
    R1_Buffer[0] = 0xFF;
    SD_Clear_SD_Receive_Buffer_Big();

    if(SD_Get_CardType() != SD_CARD_TYPE_SDHC)
    {
        block <<= 9;
    }
    Global_message.Argument = block;

    SD_CMDSPI_send_read(PGlobal_message,R1,R1_Buffer,NO);
    SD_Timeout = 0;
    while(R1_Buffer[0] != 0)
    {
        while(!SPI_WriteRead(0xFF,R1_Buffer,YES))
        {
            if(SD_CardPresent() == FAIL)
            {
                return FAIL;
            }
            SD_Timeout++;
            if(SD_Timeout >= SD_TIMEOUT_MAX)
            {
                SD_CS_INACTIVE();
                return FAIL;
            }
        }
    }

     // wait for data token
    SD_Timeout = 0;
    R1_Buffer[0] = 0xFF;
    while(R1_Buffer[0] != DATA_START_BLOCK)
    {
        while(!SPI_WriteRead(0xFF,R1_Buffer,YES))
        {
            if(SD_CardPresent() == FAIL)
            {
                return FAIL;
            }
            SD_Timeout++;
            if(SD_Timeout >= SD_TIMEOUT_MAX)
            {
                SD_CS_INACTIVE();
                return FAIL;
            }
        }
    }

    // read data
    BytesReadTotal = 0;
    for(j = StartIndex; j < StopIndex; j++)
    {
        /* read a sector */
        BytesRead = 0;
        SD_Clear_SD_Receive_Buffer_Big();
        Pbuf = SD_Receive_Buffer_Big;
        MISOCheck = YES;
        for (i=0; i<SDblockSize; i++)
        {
            if(SPI_WriteRead(0xFF,Pbuf,MISOCheck))
            {
                BytesRead++;
                BytesReadTotal++;
            }
            MISOCheck = NO;
            Pbuf++;
        }
        // read CRC
        SPIwrite(0xFF); // LSB
        SPIwrite(0xFF); // MSB

        /* function to use the data goes here */
        fpointer();        
        SD_Clear();
    }
    SD_CMDSPI_send_read(SD_SetCMD(CMD12,0),R1,R1_Buffer, NO);
    SD_Clear();
    SD_CS_INACTIVE();
    return PASS;
}

/******************************************************************************/
/* SD_writeBlock
 *
 * Writes an SD block and stores it in SD_Receive_Buffer_Big.
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
unsigned char SD_writeBlock(unsigned long blockIndex, unsigned char* data)
{
    unsigned int i;
    unsigned long block;

    Global_message.Transmitter = 1;
    Global_message.Command = CMD24;
    block = blockIndex;
    R1_Buffer[0] = 0xFF;

    if(SD_Get_CardType() != SD_CARD_TYPE_SDHC)
    {
        block <<= 9;
    }
    Global_message.Argument = block;

    SD_CMDSPI_send_read(PGlobal_message,R1,R1_Buffer,NO);
    SD_Timeout = 0;
    while(R1_Buffer[0] != 0)
    {
        while(!SPI_WriteRead(0xFF,R1_Buffer,YES))
        {
            if(SD_CardPresent() == FAIL)
            {
                return FAIL;
            }
            SD_Timeout++;
            if(SD_Timeout >= SD_TIMEOUT_MAX)
            {
                SD_CS_INACTIVE();
                return FAIL;
            }
        }
    }

    // send dummy
    SPIwrite(0xFF);

    // send data token
    SPIwrite(0xFE);

    // write data
    for (i=0; i<SDblockSize; i++)
    {
        SPIwrite(data[i]);
    }

    SPIwrite(0xFF); // CRC high
    SPIwrite(0xFF); // CRC low

    SD_Timeout = 0;
    while(SPI_WriteRead(0xFF,R1_Buffer,YES)) // wait for it to finish
    {
        SD_Timeout++;
        if(SD_Timeout >= SD_TIMEOUT_MAX)
        {
            SD_CS_INACTIVE();
            return FAIL;
        }
    }

    SD_CS_INACTIVE();
    return PASS;
}

/******************************************************************************/
/* SD_TestCRC
 *
 * The function initializes the CRC algorithm.
/******************************************************************************/
void SD_TestCRC(void)
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

    SPI_SetSpeed(400.0); /* set speed to 400kHz */
    SPI_Enable();
    MSC_DelayUS(1000);/* needed for SPI clock to stabalize */

    for(i=0;i<SD_INIT_MAX;i++)
    {        
        SD_RESET();

        MSC_DelayUS(1000);
        Acknowledge = SD_CMDSPI_send_read(Pmessage,R1,R1_Buffer,YES);
        if(Acknowledge == TRUE)
        {
            if(R1_Buffer[0] == R1_IDLE_STATE)
            {
                return TRUE;
            }
        }
        MSC_DelayUS(200);
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
    SD_Clear();
    SPIwrite(first);
    SPIwrite(second);
    SPIwrite(third);
    SPIwrite(forth);
    SPIwrite(fifth);
    SPIwrite(sixth);
}

/******************************************************************************/
/* SD_CMDSPI_send
 *
 * The function sends an SD card command.
/******************************************************************************/
SDcommand* SD_SetCMD(unsigned char CMD, unsigned long arguement)
{
    Global_message.Transmitter = 1;
    Global_message.Command = CMD;
    Global_message.Argument = arguement;
    SDaddCRC(PGlobal_message);
    return PGlobal_message;
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
    unsigned char response = FALSE;
    unsigned int i;
    unsigned char MISOCheck;

    SD_CMDSPI_send(message);

    SD_Timeout = 0;
    if(ResponseType == R1)
    {
        while(!SPI_WriteRead(0xFF,read,YES))
        {
            if(SD_CardPresent() == FAIL)
            {
                return FAIL;
            }
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
        MISOCheck = YES;
        for(i=0;i<ResponseType;i++)
        {
            while(!SPI_WriteRead(0xFF,read,MISOCheck))
            {
                if(SD_CardPresent() == FAIL)
                {
                    return FAIL;
                }
                SD_Timeout++;
                if(SD_Timeout >= SD_TIMEOUT_MAX)
                {
                    SD_CS_INACTIVE();
                    if(response)
                    {
                        return TRUE;
                    }
                    return FALSE;
                }
            }
            MISOCheck = NO;
            response = TRUE;
            read++;
        }
    }

    if(CSonGood)
    {
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
    MSC_DelayUS(10);
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
    SD_Timeout = 0;
    /* Write data until there is no response */
    while(SPI_WriteRead(0xFF, NULL, YES))
    {
        if(SD_CardPresent() == FAIL)
        {
            return;
        }
        SD_Timeout++;
        if(SD_Timeout >= SD_TIMEOUT_MAX)
        {
            return;
        }
    }
}

/******************************************************************************/
/* SD_Clear
 *
 * The function waits until the SD card talks.
/******************************************************************************/
unsigned char SD_WaitResponse(void)
{
    SD_Timeout = 0;
    while(!SPI_WriteRead(0xFF, NULL, YES))
    {
        if(SD_CardPresent() == FAIL)
        {
            return FAIL;
        }
        SD_Timeout++;
        if(SD_Timeout >= SD_TIMEOUT_MAX)
        {
            return FALSE;
        }
    }
    return TRUE;
}

/******************************************************************************/
/* SD_CardPresent
 *
 * The function returns true if there is an SD card inserted.
/******************************************************************************/
unsigned char SD_CardPresent(void)
{
#ifndef PROTOBOARD
    //READ RC6
    if(PORTC & SD_INSERT)
    {
        return OFF;
    }
#endif
    return ON;
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/