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

#include "USER.h"
#include "SYSTEM.h"
#include "MISC.h"
#include "SPI.h"

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
unsigned char SPIprescale[22] = {1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 28,
32, 40, 48, 56, 64, 80, 96, 112, 128};

volatile unsigned char SPI_State = FINISHED;

/******************************************************************************/
/* Inline Functions                                                           */
/******************************************************************************/

/******************************************************************************/
/* SPI_Enable
 *
 * Turns on SPI module.
/******************************************************************************/
inline void SPI_Enable(void)
{
    SPI2STATbits.SPIEN = 1; // Turn on SPI module
    IFS2bits.SPI2IF = 0;    // Clear the Interrupt flag
    IEC2bits.SPI2IE = 1;    // Disable the interrupt
}

/******************************************************************************/
/* SPI_Disable
 *
 * Turns off SPI module.
/******************************************************************************/
inline void SPI_Disable(void)
{
    SPI2STATbits.SPIEN = 0; // Turn off module
    IFS2bits.SPI2IF = 0; // Clear the Interrupt flag
    IEC2bits.SPI2IE = 0; // Disable the interrupt
}

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
    SPI_Disable();

    RPINR22bits.SDI2R = SPI_SD_MISO_RP; // SPI2 data input (MISO) is set to RP2
#ifdef PROTOBOARD
    RPOR3bits.RP7R = 0x0A;    // RP7 = SDO aka MOSI
    RPOR4bits.RP8R = 0x0B;    // RP8 = SCK
#else
    RPOR3bits.RP7R = 0x0B;    // RP8 = SDO aka MOSI
    RPOR4bits.RP8R = 0x0A;    // RP7 = SCK
#endif

    SPI2CON1bits.MODE16 = FALSE;
    SPI2CON1bits.MSTEN = TRUE; /* Master mode */
    /* set to SPI mode 0 */
    SPI2CON1bits.CKE = 0;
    SPI2CON1bits.CKP = 1;
    SPI2CON1bits.SMP = 1;

    SPI_SetSpeed(400.0); /* set speed to 400kHz */

    SPI_Enable();
    MSC_DelayUS(1000); /* needed for SPI clock to stabalize */
}

/******************************************************************************/
/* SPI_SetSpeed
 *
 * The function sets the speed in kHz on the SPI bus.
/******************************************************************************/
void SPI_SetSpeed(double kHz)
{
    unsigned char i;
    unsigned int prescale;
    unsigned long FCYtemp = FCY;

    SPI_Disable();
    prescale = (unsigned int) MSC_DB_Round((double) FCYtemp / (kHz*1000.0));

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
    
    SPI_Disable();
    
    switch (prescale)
    {
        case 1:
            SPI2CON1bits.SPRE = 0x7; /* 1:1 */
            SPI2CON1bits.PPRE = 0x3; /* 1:1 */
            break;
        case 2:
            SPI2CON1bits.SPRE = 0x6; /* 2:1 */
            SPI2CON1bits.PPRE = 0x3; /* 1:1 */
            break;
        case 3:
            SPI2CON1bits.SPRE = 0x5; /* 3:1 */
            SPI2CON1bits.PPRE = 0x3; /* 1:1 */
            break;
        case 4:
            SPI2CON1bits.SPRE = 0x4; /* 4:1 */
            SPI2CON1bits.PPRE = 0x3; /* 1:1 */
            break;
        case 5:
            SPI2CON1bits.SPRE = 0x3; /* 5:1 */
            SPI2CON1bits.PPRE = 0x3; /* 1:1 */
            break;
        case 6:
            SPI2CON1bits.SPRE = 0x2; /* 6:1 */
            SPI2CON1bits.PPRE = 0x3; /* 1:1 */
            break;
        case 7:
            SPI2CON1bits.SPRE = 0x1; /* 7:1 */
            SPI2CON1bits.PPRE = 0x3; /* 1:1 */
            break;
        case 8:
            SPI2CON1bits.SPRE = 0x0; /* 8:1 */
            SPI2CON1bits.PPRE = 0x3; /* 1:1 */
            break;
        case 12:
            SPI2CON1bits.SPRE = 0x5; /* 3:1 */
            SPI2CON1bits.PPRE = 0x2; /* 4:1 */
            break;
        case 16:
            SPI2CON1bits.SPRE = 0x4; /* 4:1 */
            SPI2CON1bits.PPRE = 0x2; /* 4:1 */
            break;
        case 20:
            SPI2CON1bits.SPRE = 0x3; /* 5:1 */
            SPI2CON1bits.PPRE = 0x2; /* 4:1 */
            break;
        case 24:
            SPI2CON1bits.SPRE = 0x2; /* 6:1 */
            SPI2CON1bits.PPRE = 0x2; /* 4:1 */
            break;
        case 28:
            SPI2CON1bits.SPRE = 0x1; /* 7:1 */
            SPI2CON1bits.PPRE = 0x2; /* 4:1 */
            break;
        case 32:
            SPI2CON1bits.SPRE = 0x0; /* 8:1 */
            SPI2CON1bits.PPRE = 0x2; /* 4:1 */
            break;
        case 40:
            SPI2CON1bits.SPRE = 0x3; /* 5:1 */
            SPI2CON1bits.PPRE = 0x1; /* 8:1 */
            break;
        case 48:
            SPI2CON1bits.SPRE = 0x2; /* 6:1 */
            SPI2CON1bits.PPRE = 0x1; /* 8:1 */
            break;
        case 56:
            SPI2CON1bits.SPRE = 0x1; /* 7:1 */
            SPI2CON1bits.PPRE = 0x1; /* 8:1 */
            break;
        case 64:
            SPI2CON1bits.SPRE = 0x0; /* 8:1 */
            SPI2CON1bits.PPRE = 0x1; /* 8:1 */
            break;
        case 80:
            SPI2CON1bits.SPRE = 0x3; /* 5:1 */
            SPI2CON1bits.PPRE = 0x0; /* 16:1 */
            break;
        case 96:
            SPI2CON1bits.SPRE = 0x2; /* 6:1 */
            SPI2CON1bits.PPRE = 0x0; /* 16:1 */
            break;
        case 112:
            SPI2CON1bits.SPRE = 0x1; /* 7:1 */
            SPI2CON1bits.PPRE = 0x0; /* 16:1 */
            break;
        default:
        case 128:
            SPI2CON1bits.SPRE = 0x0; /* 8:1 */
            SPI2CON1bits.PPRE = 0x0; /* 16:1 */
            break;
    }
}

/******************************************************************************/
/* SPI_ReadActivityMISO
 *
 * Reads the MISO line to check for activity.
/******************************************************************************/
void SPI_ReadActivityMISO(unsigned char* event)
{
    if(*event == FALSE)
    {
        if((PORTB & SPI_SD_MISO2) == 0)
        {
            *event = TRUE;
        }
    }
}

/******************************************************************************/
/* SPI_WriteRead
 *
 * The function writes 2 bytes over the SPI and listens for a response.
/******************************************************************************/
unsigned char SPI_WriteRead(unsigned char write, unsigned char* read,unsigned char CheckActivity)
{
    unsigned int dummy;
    unsigned long timer=0;
    unsigned char MISO_event = FALSE;
    /* dummy read */
    dummy = SPI2BUF;
    dummy = SPI2BUF;
    dummy = SPI2BUF;

    /* Errata workaround */
    SPI_State = NOTFINISHED;    
    while(!SPI2STATbits.SPITBF)
    {
        SPI2BUF = write;
        Nop();
        Nop();
    }
    while(SPI2STATbits.SPITBF);
    while(SPI_State != FINISHED)
    {
        timer++;
        if(SD_CardPresent() == FAIL)
        {
            return 0;
        }
        SPI_ReadActivityMISO(&MISO_event);
        if(timer>SPI_TIMEOUT_COUNTS || !SD_CardPresent())
        {
            return 0;
        }
    }

    dummy = SPI2BUF;
    SPI2STATbits.SPIROV = FALSE;

    if(SD_CardPresent() == FAIL)
    {
        return 0;
    }
    if(CheckActivity)
    {
        if(MISO_event)
        {
            *read = dummy;
            return 1;
        }
    }
    else
    {
        *read = dummy;
        return 1;
    }
    return 0;
}

/******************************************************************************/
/* SPIwrite
 *
 * The function writes 2 bytes over the SPI bus without listening.
/******************************************************************************/
void SPIwrite(unsigned char write)
{
    SPI_WriteRead(write,NULL,NO);
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/