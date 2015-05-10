/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 05/09/15     1.0_DW0     Initial project make.
/******************************************************************************/

/******************************************************************************/
/* Contains Functions for PIC initialization
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
#include "MISC.h"
#include "Sound.h"


/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

/******************************************************************************/
/* Functions
/******************************************************************************/

/******************************************************************************/
/* InitApp
 *
 * The function initializes the Digital to analog convertor.
/******************************************************************************/
void InitDAC(void)
{
DAC1STATbits.ROEN = 1; /* Right Channel DAC Output Enabled */
DAC1STATbits.RITYPE = 0; /* Right Channel Interrupt if FIFO is not Full */
DAC1CONbits.AMPON = 0; /* Amplifier Disabled During Sleep and Idle Modes */
DAC1CONbits.DACFDIV = 7; /* Divide Clock by 1 (Assumes Clock is 25.6MHz) */
DAC1CONbits.FORM = 0; /* Data Format is Unsigned */
DAC1DFLT = 0x8000; /* Default value set to Midpoint when FORM = 0 */
IFS4bits.DAC1RIF = 0; /* Clear Right Channel Interrupt Flag */
IEC4bits.DAC1RIE = 1; /* Right Channel Interrupt Enabled */
DAC1CONbits.DACEN = 1; /* DAC1 Module Enabled */

}

/******************************************************************************/
/* TurnOnAmp
 *
 * The function turns on the audio amp.
/******************************************************************************/
void TurnOnAmp(void)
{
    LATA |= AudioAmpStandby;
}

/******************************************************************************/
/* TurnOffAmp
 *
 * The function turns off the audio amp.
/******************************************************************************/
void TurnOffAmp(void)
{
    LATA &= ~AudioAmpStandby;
}

/******************************************************************************/
/* MuteAmp
 *
 * The function mutes the audio amp.
/******************************************************************************/
void MuteAmp(void)
{
    LATB &= ~AudioAmpMute;
}

/******************************************************************************/
/* MuteAmp
 *
 * The function mutes the audio amp.
/******************************************************************************/
void UnMuteAmp(void)
{
    LATB |= AudioAmpMute;
}

/******************************************************************************/
/* DAC_Voltage
 *
 * The function mutes the audio amp.
/******************************************************************************/
void DAC_Voltage(unsigned int counts)
{
    //DAC1RDAT = counts;
}


/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/