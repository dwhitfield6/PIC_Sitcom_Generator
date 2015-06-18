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
#define R16 16
#define R512 512

/******************************************************************************/
/* Commands                                                                   */
/******************************************************************************/
#define CMD0 0      // resets the SD card
#define CMD1 1
#define CMD8 8
#define CMD9 9      // sends card-specific data
#define CMD10 10    // sends card identification
#define CMD17 17    // reads a block at byte address
#define CMD24 24    // writes a block at byte address
#define CMD55 55    // application command
#define CMD41 41    // starts card initialization

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
/* User Global Variable Declaration                                           */
/******************************************************************************/
extern unsigned char SD_Initialized;
extern unsigned char ReceiveBuffer;

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
unsigned char SD_CMDSPI_send_read(SDcommand* message, unsigned char ResponseType, unsigned char* read);
void SD_CMDSPI_send(SDcommand* message);
void SDaddCRC(SDcommand* message);
unsigned char CRC7_40bits(unsigned long MSBmessage, unsigned long LSBmessage, unsigned char mask);
void SD_RESET(void);
void Clear_Receive_Buffer(void);
unsigned char SD_readRegister(SDcommand* pmessage);
unsigned char SD_readBlock(long blockIndex);

#endif	/* SD_H */