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
 *                          Fixed bugs.
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
#include "WAV.h"

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/

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
    int temp;
    IFS4bits.DAC1RIF = 0; /* Clear Right Channel Interrupt Flag */
    if(StartupSong)
    {
        temp = Start_Clip[DAC_Buffer_Place];
        if(temp != 0)
        {
            DAC_Buffer_Place++;
            temp -= 127; //Turn from unsigned to signed
            DAC1RDAT = temp << 7;
        }
        else
        {
            DAC_Buffer_Place = 0;
            StartupSong = FALSE;
            ClipDone = TRUE;
            DAC1RDAT = 0x8000;
        }
    }
    else
    {
        if(DAC_Buffer_Place >= DAC_Buffer_Elements)
        {
            if(WAV_DONE)
            {
                ClipDone = TRUE;
            }
            DAC_Buffer_Place = 0;
            if(DAC_Page_Read == FIRST)
            {
                DAC_Page_Read = SECOND;
            }
            else
            {
                DAC_Page_Read = FIRST;
            }
        }
        else
        {
            temp = DAC_FIFO[DAC_Page_Read][DAC_Buffer_Place];
            DAC1RDAT = temp;
            DAC_Buffer_Place++;
        }
    }

    if(ClipDone == TRUE)
    {
        DAC_OFF();
    }
}

/******************************************************************************/
/* DAC interrupt
/******************************************************************************/
void _ISR_NOPSV _SPI2Interrupt(void)
{
    IFS2bits.SPI2IF = 0;    // Clear the Interrupt flag
    SPI_State = FINISHED;
}

/******************************************************************************/
/* DAC interrupt
/******************************************************************************/
void _ISR_NOPSV _INT1Interrupt(void)
{
    IFS1bits.INT1IF = 0;    // Clear the Interrupt flag
    SPI_State = FINISHED;
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/