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
/* User Global Variable Declaration                                           */
/******************************************************************************/
extern volatile unsigned char SPI_State;

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/
#define FINISHED    10
#define GOING       9
#define NOTFINISHED 9
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
void SetSPISpeed(double kHz);
unsigned char SPIwrite_read(unsigned char write, unsigned char* read);
void SPIwrite(unsigned char write);
inline void SPI_Enable(void);
inline void SPI_Disable(void);

#endif	/* SPI_H */