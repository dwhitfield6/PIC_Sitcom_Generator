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
/* Contains Functions for the SPI bus.
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
#include "system.h"
#include "MISC.h"
#include "SPI.h"

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
unsigned char SPIprescale[22] = {1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 28,
32, 40, 48, 56, 64, 80, 96, 112, 128};

/******************************************************************************/
/* Inline Functions
/******************************************************************************/

/******************************************************************************/
/* Functions
/******************************************************************************/

/******************************************************************************/
/* InitSPI
 *
 * The function initializes the SPI bus.
/******************************************************************************/
void InitSPI(void)
{
    RPINR20bits.SDI1R = 0x02; // SPI1 data input (MISO) is set to RP2
    RPOR3bits.RP7R = 0x07;    // RP7 = SDO aka MOSI
    RPOR4bits.RP8R = 0x08;    // RP8 = SDO aka SCK
    SPI1CON1bits.MODE16 = FALSE;
    SetSPISpeed(400.0); /* set speed to 400kHz */
    /* set to SPI mode 0 */
    SPI1CON1bits.CKE = 0;
    SPI1CON1bits.CKP = 0;
    SPI1CON1bits.SMP = 0;

    SPI1CON1bits.MSTEN = TRUE; /* Master mode */

    SPI1STATbits.SPIEN = TRUE; /* module is enabled and configures the
                                * pins as serial port pins
                                */

}

/******************************************************************************/
/* SetSPISpeed
 *
 * The function sets the speed in kHz on the SPI bus.
/******************************************************************************/
void SetSPISpeed(double kHz)
{
    unsigned char i;
    unsigned int prescale;
    unsigned long FCYtemp = FCY;

    prescale = (unsigned int) DBround((double) FCYtemp / (kHz*1000.0));

    if(prescale < 1)
    {
        prescale = 1;
    }
    else if(prescale >= 120)
    {
        prescale = 128;
    }

    if(prescale != 128 && prescale != 1)
    {
        for(i = 0; i< 21; i++)
        {
            if(prescale >= (SPIprescale[i] << 1) && prescale < (SPIprescale[i+1] << 1))
            {
                prescale = (unsigned char) SPIprescale[i];
                break;
            }
        }
    }
    if(prescale == 1)
    {
        prescale = 2; /* the SPI bus can not operate this fast */
    }
    switch (prescale)
    {
        case 1:
            SPI1CON1bits.SPRE = 0x7; /* 1:1 */
            SPI1CON1bits.PPRE = 0x3; /* 1:1 */
            break;
        case 2:
            SPI1CON1bits.SPRE = 0x6; /* 2:1 */
            SPI1CON1bits.PPRE = 0x3; /* 1:1 */
            break;
        case 3:
            SPI1CON1bits.SPRE = 0x5; /* 3:1 */
            SPI1CON1bits.PPRE = 0x3; /* 1:1 */
            break;
        case 4:
            SPI1CON1bits.SPRE = 0x4; /* 4:1 */
            SPI1CON1bits.PPRE = 0x3; /* 1:1 */
            break;
        case 5:
            SPI1CON1bits.SPRE = 0x3; /* 5:1 */
            SPI1CON1bits.PPRE = 0x3; /* 1:1 */
            break;
        case 6:
            SPI1CON1bits.SPRE = 0x2; /* 6:1 */
            SPI1CON1bits.PPRE = 0x3; /* 1:1 */
            break;
        case 7:
            SPI1CON1bits.SPRE = 0x1; /* 7:1 */
            SPI1CON1bits.PPRE = 0x3; /* 1:1 */
            break;
        case 8:
            SPI1CON1bits.SPRE = 0x0; /* 8:1 */
            SPI1CON1bits.PPRE = 0x3; /* 1:1 */
            break;
        case 12:
            SPI1CON1bits.SPRE = 0x5; /* 3:1 */
            SPI1CON1bits.PPRE = 0x2; /* 4:1 */
            break;
        case 16:
            SPI1CON1bits.SPRE = 0x4; /* 4:1 */
            SPI1CON1bits.PPRE = 0x2; /* 4:1 */
            break;
        case 20:
            SPI1CON1bits.SPRE = 0x3; /* 5:1 */
            SPI1CON1bits.PPRE = 0x2; /* 4:1 */
            break;
        case 24:
            SPI1CON1bits.SPRE = 0x2; /* 6:1 */
            SPI1CON1bits.PPRE = 0x2; /* 4:1 */
            break;
        case 28:
            SPI1CON1bits.SPRE = 0x1; /* 7:1 */
            SPI1CON1bits.PPRE = 0x2; /* 4:1 */
            break;
        case 32:
            SPI1CON1bits.SPRE = 0x0; /* 8:1 */
            SPI1CON1bits.PPRE = 0x2; /* 4:1 */
            break;
        case 40:
            SPI1CON1bits.SPRE = 0x3; /* 5:1 */
            SPI1CON1bits.PPRE = 0x1; /* 8:1 */
            break;
        case 48:
            SPI1CON1bits.SPRE = 0x2; /* 6:1 */
            SPI1CON1bits.PPRE = 0x1; /* 8:1 */
            break;
        case 56:
            SPI1CON1bits.SPRE = 0x1; /* 7:1 */
            SPI1CON1bits.PPRE = 0x1; /* 8:1 */
            break;
        case 64:
            SPI1CON1bits.SPRE = 0x0; /* 8:1 */
            SPI1CON1bits.PPRE = 0x1; /* 8:1 */
            break;
        case 80:
            SPI1CON1bits.SPRE = 0x3; /* 5:1 */
            SPI1CON1bits.PPRE = 0x0; /* 16:1 */
            break;
        case 96:
            SPI1CON1bits.SPRE = 0x2; /* 6:1 */
            SPI1CON1bits.PPRE = 0x0; /* 16:1 */
            break;
        case 112:
            SPI1CON1bits.SPRE = 0x1; /* 7:1 */
            SPI1CON1bits.PPRE = 0x0; /* 16:1 */
            break;
        default:
        case 128:
            SPI1CON1bits.SPRE = 0x0; /* 8:1 */
            SPI1CON1bits.PPRE = 0x0; /* 16:1 */
            break;
    }
}

/******************************************************************************/
/* SPIwrite_read
 *
 * The function initializes the SPI bus.
/******************************************************************************/
unsigned char SPIwrite_read(unsigned char write)
{
    SPI1BUF = write;
    while(SPI1STATbits.SPITBF); /* wait for the send to finish */
    //while(SPI1STATbits.SPIRBF); /* wait for the receive to finish */
    return SPI1BUF;
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/