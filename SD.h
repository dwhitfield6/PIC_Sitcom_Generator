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
#define SD_INIT_MAXATTEMPTS 20

/******************************************************************************/
/* Commands                                                                   */
/******************************************************************************/
#define CMD0 0
#define CMD1 1
#define CMD8 1
#define CMD55 1
#define CMD41 1

/******************************************************************************/
/* R1 Responses                                                               */
/******************************************************************************/
#define GOOD_NOT_IDLE   0x00
#define IDLE_STATE      0x01
#define ERASE_RESET     0x02
#define ILLEGAL_COMMAND 0x04
#define CRC_ERROR       0x08
#define ERASE_SEQ_ERROR 0x10
#define ADDRESS_ERROR   0x20
#define PARAMETER_ERROR 0x40

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
extern unsigned char SD_Initialized;

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
unsigned char SDsendCMDSPI(SDcommand* message, unsigned char ReadCycles, unsigned char* read);
void SDaddCRC(SDcommand* message);
unsigned char CRC7_40bits(unsigned long MSBmessage, unsigned long LSBmessage, unsigned char mask);
void SD_RESET(void);

#endif	/* SD_H */