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
#include "DAC.h"

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
unsigned char DAC_Res = _8BIT;
volatile unsigned char ClipDone = FALSE;
volatile unsigned char StartupSong = TRUE;
/******************************************************************************/
/* Inline Functions
/******************************************************************************/

/******************************************************************************/
/* DAC_Run
 *
 * This function enables the DAC interrupt so that sound starts playing.
/******************************************************************************/
inline void DAC_Run(void)
{
    IFS4bits.DAC1RIF = 0; /* Clear Right Channel Interrupt Flag */
    IEC4bits.DAC1RIE = 1; /* Right Channel Interrupt Enabled */
}

/******************************************************************************/
/* DAC_Stop
 *
 * This function disables the DAC interrupt so that sound stops playing.
/******************************************************************************/
inline void DAC_Stop(void)
{
    IFS4bits.DAC1RIF = 0; /* Clear Right Channel Interrupt Flag */
    IEC4bits.DAC1RIE = 0; /* Right Channel Interrupt disabled */
}

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
    ACLKCONbits.APSTSCLR = 5; // Auxiliary Clock Output Divider is 4
    DAC1STATbits.ROEN = 1; /* Right Channel DAC Output Enabled */
    DAC1STATbits.RITYPE = 0; /* Right Channel Interrupt if FIFO is not Full */
    DAC1CONbits.AMPON = 0; /* Amplifier Disabled During Sleep and Idle Modes */
    DAC1CONbits.DACFDIV = 7; /* Divide Clock by 1 (Assumes Clock is 25.6MHz) */
    DAC1CONbits.FORM = 0; /* Data Format is Unsigned */
    DAC1DFLT = 0x8000; /* Default value set to Midpoint when FORM = 0 */
}

/******************************************************************************/
/* TurnOnAmp
 *
 * The function turns on the audio amp.
/******************************************************************************/
void TurnOnAmp(void)
{
    LATB |= AudioAmpStandby;
}

/******************************************************************************/
/* TurnOffAmp
 *
 * The function turns off the audio amp.
/******************************************************************************/
void TurnOffAmp(void)
{
    LATB &= ~AudioAmpStandby;
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
    DAC1RDAT = counts;
}

/******************************************************************************/
/* AudioON
 *
 * The function turns on the amp and starts the DAC sampling.
/******************************************************************************/
void AudioOn(void)
{
    DAC_Stop();
    TurnOnAmp();
    delayUS(1000);
    //UnMuteAmp();
    DAC_Run();
    DAC_ON();
}

/******************************************************************************/
/* AudioOff
 *
 * The function stops the DAC and powes down the audio amp.
/******************************************************************************/
void AudioOff(void)
{
    MuteAmp();
    DAC_Stop();
    DAC_OFF();
    TurnOffAmp();
}

/******************************************************************************/
/* PlayStartup
 *
 * The function plays "Sitcom Generator On" over the speaker.
/******************************************************************************/
void Play_Startup(void)
{
    DAC_Res = _8BIT;
    ClipDone = FALSE;
    AudioOn();
    StartupSong = TRUE;
    while(!ClipDone); /* wait till it stops playing */
}

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/