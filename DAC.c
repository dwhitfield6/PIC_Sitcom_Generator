/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 05/09/15     1.0_DW0a    Initial project make.
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
volatile unsigned char ClipDone = FALSE;
volatile unsigned char StartupSong = TRUE;
unsigned int DAC_FIFO[2][DAC_BUFFER_SIZE]; /* double buffer */ /* TODO implement wav play so that the buffer does not overflow for 8 bit of mono data */
volatile unsigned char DAC_Page_Write;
volatile unsigned char DAC_Page_Read;
unsigned int DAC_Buffer_Place = 0;
unsigned int DAC_Buffer_Elements[2];
volatile unsigned char DAC_Page_Write_Finished[2];

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
    IEC4bits.DAC1RIE = 1; /* Right Channel Interrupt Enabled */
}

/******************************************************************************/
/* DAC_Stop
 *
 * This function disables the DAC interrupt so that sound stops playing.
/******************************************************************************/
inline void DAC_Stop(void)
{
    IEC4bits.DAC1RIE = 0; /* Right Channel Interrupt disabled */
}

/******************************************************************************/
/* DAC_ToggleWriteDACPage
 *
 * The function switchs to he other page in the double buffer.
/******************************************************************************/
inline void DAC_ToggleWriteDACPage(void)
{
    if(DAC_Page_Write == FIRST)
    {
        DAC_Page_Write = SECOND;
    }
    else
    {
        DAC_Page_Write = FIRST;
    }
    while(DAC_Page_Write_Finished[DAC_Page_Write] == TRUE); /* wait for read to finish on the page */
}

/******************************************************************************/
/* Functions
/******************************************************************************/

/******************************************************************************/
/* Init_App
 *
 * The function initializes the Digital to analog convertor.
/******************************************************************************/
void InitDAC(void)
{
    DAC_AudioOff();
    ACLKCONbits.APSTSCLR = 0x7; // Auxiliary Clock Output Divider is 1
    DAC1STATbits.ROEN = 1; /* Right Channel DAC Output Enabled */
    DAC1STATbits.RITYPE = 0; /* Right Channel Interrupt if FIFO is not Full */
    IPC19bits.DAC1RIP = 5; // Interrupt priority is 5
    DAC1CONbits.AMPON = 0; /* Amplifier Disabled During Sleep and Idle Modes */
    DAC_SetClock(8000);
    DAC1CONbits.FORM = 1; /* Data Format is Signed */
    DAC1DFLT = 0x0000; /* Default value set to Midpoint when FORM = 0 */
    DAC_Run();
}

/******************************************************************************/
/* TurnOnAmp
 *
 * The function turns on the audio amp.
/******************************************************************************/
void DAC_TurnOnAmp(void)
{
    LATB |= AudioAmpStandby;
}

/******************************************************************************/
/* DAC_SetClock
 *
 * The function sets the DAC playback speed in Hz.
/******************************************************************************/
void DAC_SetClock(double Speed)
{
    unsigned long div;

    div = (unsigned long)((double)FOSC /(Speed * 128));
    div--;
    
    DAC1CONbits.DACFDIV = div; /* Divide Clock by 1 (Assumes Clock is 25.6MHz) */
}

/******************************************************************************/
/* TurnOffAmp
 *
 * The function turns off the audio amp.
/******************************************************************************/
void DAC_TurnOffAmp(void)
{
    LATB &= ~AudioAmpStandby;
}

/******************************************************************************/
/* MuteAmp
 *
 * The function mutes the audio amp.
/******************************************************************************/
void DAC_MuteAmp(void)
{
    LATB &= ~AudioAmpMute;
}

/******************************************************************************/
/* MuteAmp
 *
 * The function mutes the audio amp.
/******************************************************************************/
void DAC_UnMuteAmp(void)
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
void DAC_AudioOn(void)
{
    DAC_TurnOnAmp();
    MSC_DelayUS(10000);
    DAC_UnMuteAmp();
    MSC_DelayUS(1000);
}

/******************************************************************************/
/* AudioOff
 *
 * The function stops the DAC and powes down the audio amp.
/******************************************************************************/
void DAC_AudioOff(void)
{
    DAC_MuteAmp();
    DAC_TurnOffAmp();
}

/******************************************************************************/
/* PlayStartup
 *
 * The function plays "Sitcom Generator On" over the speaker.
/******************************************************************************/
void DAC_Play_Startup(void)
{
    DAC_Buffer_Place = 0; /* start at beginning of buffer  */
    DAC_AudioOn();
    StartupSong = TRUE;
    ClipDone = FALSE;
    DAC_ON();   
    while(!ClipDone); /* wait till it stops playing */
    DAC_AudioOff();
}

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/