/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 05/28/15     1.0_DW0a    Initial coding.
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#ifndef SD_H
#define	SD_H

#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

/******************************************************************************/
/* Structures                                                                 */
/******************************************************************************/
typedef struct sdproperties
{
    unsigned char CSDVer;   // CSR version
    unsigned long Size;     // rounded size that is printed on card
    unsigned long RawSize;  // The raw size that is avaiable to use
    unsigned int ReadLength;
    unsigned int WriteLength;
    unsigned int Speed;
    unsigned int CCC;       // card command classes
    unsigned char PERM_WP;  // Permananent Write Protection
    unsigned char WP;       // Temporary write Protection
    unsigned long Blocks;       // Temporary write Protection
}SDproperties;

typedef struct sd
{
    unsigned char Transmitter;
    unsigned char Command;
    unsigned long Argument;
    unsigned char CRC;
}SDcommand;

/******************************************************************************/
/* SDbufSize
 *
 * This is the size of a block of memory in the sd card.
/******************************************************************************/
# define SDblockSize 512

/******************************************************************************/
/* WriteProtect
 * 
 * Define this if the SD card holder has a Write protect switch.
/******************************************************************************/
//#define WriteProtect

/******************************************************************************/
/* CardDetect
 * 
 * Define this if the SD card holder has a Card insertion switch.
/******************************************************************************/
//#define CardDetect

/******************************************************************************/
/* CRCMASK_7
 * 
 * Mask for the sd crc polyomial. x^7 + x^3 + 1
/******************************************************************************/
#define CRCMASK_7 0x89

/******************************************************************************/
/* SD_INIT_MAXATTEMPTS
 * 
 * This is the maximum attempts that we try to put the sd card into SPI mode.
/******************************************************************************/
#define SD_INIT_MAX 255

/******************************************************************************/
/* SD_TIMEOUT
 *
 * This is the maximum attempts that we attempt to get a response from the card
/******************************************************************************/
#define SD_TIMEOUT_MAX 255

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/
#define R1 1
#define R2 2
#define R3 3
#define R6 6
#define R16 20
#define R512 512
#define INITIALIZED  1
#define RUNNING      2

/******************************************************************************/
/* Commands                                                                   */
/******************************************************************************/
// SD card commands

#define CMD0   0x00 /* GO_IDLE_STATE - init card in spi mode if CS low */
#define CMD8   0x08 /* SEND_IF_COND - verify SD Memory Card interface operating condition.*/
#define CMD9   0x09 /* SEND_CSD - read the Card Specific Data (CSD register) */
#define CMD10  0x0A /* SEND_CID - read the card identification information (CID register) */
#define CMD12  0x0C /* STOP_TRANSMISSION - Forces the card to stop transmission in Multiple Block Read Operation */
#define CMD13  0x0D /* SEND_STATUS - read the card status register */
#define CMD16  0x10 /* SET BLOCK LENGTH - set the length of the block */
#define CMD17  0x11 /* READ_BLOCK - read a single data block from the card */
#define CMD18  0x12 /* READ_MULT_BLOCK - Continuously reads SD blocks until CMD12 is sent */
#define CMD24  0x18 /* WRITE_BLOCK - write a single data block to the card */
#define CMD25  0x19 /* WRITE_MULTIPLE_BLOCK - write blocks of data until a STOP_TRANSMISSION */
#define CMD32  0x20 /* ERASE_WR_BLK_START - sets the address of the first block to be erased */
#define CMD33  0x21 /* ERASE_WR_BLK_END - sets the address of the last block of the continuous
                      * range to be erased
                      */

#define CMD38  0x26 /* ERASE - erase all previously selected blocks */
#define CMD55  0x37 /* APP_CMD - escape for application specific command */
#define CMD56  0x38 /* GEN_CMD - escape for generic command  */
#define CMD58  0x3A /* READ_OCR - read the OCR register of a card */
#define ACMD23 0x17 /* SET_WR_BLK_ERASE_COUNT - Set the number of write blocks to be
                      * pre-erased before writing
                      */

#define ACMD41 0x29 /* SD_SEND_OP_COMD - Sends host capacity support information and
                      * activates the card's initialization process
                      */

/******************************************************************************/
/* R1 Responses                                                               */
/******************************************************************************/
#define R1_GOOD_NOT_IDLE   0x00
#define R1_IDLE_STATE      0x01
#define R1_ERASE_RESET     0x02
#define R1_ILLEGAL_COMMAND 0x04
#define R1_CRC_ERROR       0x08
#define R1_ERASE_SEQ_ERROR 0x10
#define R1_ADDRESS_ERROR   0x20
#define R1_PARAMETER_ERROR 0x40

/******************************************************************************/
/* Status Codes                                                               */
/******************************************************************************/
#define DATA_START_BLOCK        0xFE  /* start data token for read or write single block*/
#define STOP_TRAN_TOKEN         0XFD; /* stop token for write multiple blocks*/
#define WRITE_MULTIPLE_TOKEN    0XFC; /* start data token for write multiple blocks*/
#define DATA_RES_MASK           0X1F; /* mask for data response tokens after a write block operation */
#define DATA_RES_ACCEPTED       0X05; /* write data accepted token */

/******************************************************************************/
/* SD card types                                                              */
/******************************************************************************/
#define NOT_INITIALIZED    0x00
#define SD_CARD_TYPE_SD1   0x01 /* Standard capacity V1 SD card */
#define SD_CARD_TYPE_SD2   0x02 /* Standard capacity V2 SD card */
#define SD_CARD_TYPE_SDHC  0x03 /* High capacity SD card */
#define NOT_SUPPORTED      0x04 /* High capacity SD card */

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
extern volatile unsigned char SD_State;
extern unsigned char ReceiveBuffer;
extern SDcommand Global_message;
extern SDcommand* PGlobal_message;
extern SDproperties SD;
extern unsigned char Receive_Buffer_Big[SDblockSize];

/******************************************************************************/
/* SD_CS_INACTIVE
 *
 * The function Asserts the chip select pin.
/******************************************************************************/
#define SD_CS_ACTIVE() (LATB &= ~SPI_SD_CS)

/******************************************************************************/
/* SD_CS_ACTIVE
 *
 * The function Asserts the chip select pin.
/******************************************************************************/
#define SD_CS_INACTIVE() (LATB |= SPI_SD_CS)

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/
void InitSD(void);
void TestCRC(void);
unsigned char SDtoSPI(void);
unsigned char SD_CMDSPI_send_read(SDcommand* message, unsigned int ResponseType, unsigned char* read, unsigned char CSonGood);
unsigned char SD_ACMDSPI_send_read(SDcommand* message, unsigned int ResponseType, unsigned char* read);
void SD_CMDSPI_send(SDcommand* message);
void SDaddCRC(SDcommand* message);
unsigned char CRC7_40bits(unsigned long MSBmessage, unsigned long LSBmessage, unsigned char mask);
void SD_RESET(void);
void Clear_Receive_Buffer_Big(void);
void Clear_Receive_Buffer_Small(void);
unsigned char SD_readRegister(SDcommand* pmessage);
unsigned char SD_readBlock(long blockIndex);
unsigned char SD_writeBlock(long blockIndex, unsigned char* data);
void SD_Set_CardType(unsigned char type);
unsigned char SD_Get_CardType(void);
void SD_Clear(void);
unsigned char SD_WaitResponse(void);
unsigned char SD_Properties(void);
SDcommand* SD_SetCMD(unsigned char CMD, unsigned long arguement);
unsigned char SD_readStatus(void);
void SD_DeleteCard(void);
unsigned char SD_readMultipleBlock(long StartIndex, long StopIndex, void (*fpointer)(void));

#endif	/* SD_H */