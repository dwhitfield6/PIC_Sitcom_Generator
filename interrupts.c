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

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/

#define _ISR_PSV __attribute__((interrupt, __auto_psv))
#define _ISR_NOPSV __attribute__((interrupt, no_auto_psv))

unsigned long place =0;

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/
void _ISR_NOPSV _DAC1RInterrupt(void)
{
    static unsigned long i = 0;

    IFS4bits.DAC1RIF = 0; /* Clear Right Channel Interrupt Flag */
    if(StartupSong)
    {
        if(Start_Clip[i] != 0)
        {
            i++;
            DAC1RDAT = (Start_Clip[i]) << 7;
        }
        else
        {
                    DAC1RDAT = 0x8000;
            i = 0;
            //StartupSong = FALSE;
            //ClipDone = TRUE;
        }
    }
    else
    {

    }

    if(ClipDone == TRUE)
    {

        //AudioOff();
    }
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/