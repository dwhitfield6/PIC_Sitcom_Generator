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

#endif	/* SPI_H */