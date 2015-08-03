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
#ifndef SPI_H
#define	SPI_H

#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

/******************************************************************************/
/* SPI_TIMEOUT_COUNTS
 *
 * This is the conts that the spi module will wait for a response.
/******************************************************************************/
#define SPI_TIMEOUT_COUNTS 50000
/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
extern volatile unsigned char SPI_State;

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/
#define FINISHED    10
#define GOING       9
#define NOTFINISHED 8
/******************************************************************************/
/* Macro Functions                                                            */
/******************************************************************************/

/******************************************************************************/
/* SPI_CS_ACTIVE
 *
 * The pulls the SPI chip select low.
/******************************************************************************/
#define SPI_CS_ACTIVE()  (LATB &= ~SPI_SD_CS)

/******************************************************************************/
/* SPI_CS_DEACTIVE
 *
 * The pulls the SPI chip select low.
/******************************************************************************/
#define SPI_CS_DEACTIVE()  (LATB |= SPI_SD_CS)

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/
void InitSPI(void);
void SPI_SetSpeed(double kHz);
unsigned char SPI_WriteRead(unsigned char write, unsigned char* read,unsigned char CheckActivity);
void SPIwrite(unsigned char write);
inline void SPI_Enable(void);
inline void SPI_Disable(void);
void SPI_ReadActivityMISO(unsigned char* event);

#endif	/* SPI_H */