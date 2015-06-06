/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 04/02/15     1.0_DW0a    Initial project make.
 *                          Derived from project 'PIC_Smart_Rf'.
 * 04/09/15     1.0_DW0b    Fixed bugs.
 *                          Added features.
/******************************************************************************/

/******************************************************************************/
/* Contains ISR
 *
 * Look here for the default vector names
 * c:\Program Files (x86)\Microchip\xc16\v1.11\support\dsPIC33F\gld\ 
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include <xc.h>        /* XC8 General Include File */

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#include "MISC.h"
#include "DAC.h"
#include "user.h"
#include "Start_sound.h"
#include "SPI.h"

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/
unsigned long place = 0;

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/
#define _ISR_PSV __attribute__((interrupt, __auto_psv))
#define _ISR_NOPSV __attribute__((interrupt, no_auto_psv))

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/

/******************************************************************************/
/* DAC interrupt
/******************************************************************************/
void _ISR_NOPSV _DAC1RInterrupt(void)
{
    unsigned int temp;
    IFS4bits.DAC1RIF = 0; /* Clear Right Channel Interrupt Flag */
    if(StartupSong)
    {
        temp = Start_Clip[place];
        if(temp != 0)
        {
            place++;
            DAC1RDAT = temp << 7;
        }
        else
        {
            place = 0;
            StartupSong = FALSE;
            ClipDone = TRUE;
            DAC1RDAT = 0x8000;
        }
    }
    else
    {

    }

    if(ClipDone == TRUE)
    {
        DAC_OFF();
    }
}

/******************************************************************************/
/* DAC interrupt
/******************************************************************************/
void _ISR_NOPSV _SPI1Interrupt(void)
{
    IFS0bits.SPI1IF = 0;    // Clear the Interrupt flag
    SPI_State = FINISHED;
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/