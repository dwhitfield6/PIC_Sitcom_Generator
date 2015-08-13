/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 05/09/15     1.0_DW0a    Initial project make.
 *                          Ported over fileio example from microchip.
 *                          Removed the fileio example code. This code was
 *                            garbage.
 *                          Added RTCC functionality.
 *                          Fixed DAC audio play bugs.
 *                          Added SD card code.
 *                          Added CRC code.
 *                          Fixed SPI communication.
 *                          Added idle state check for SD card.
 *                          Changed start-up sound to say "Sitcom Generator".
 *                          Added SD block and register reads.
 *                          Fixed SD card initialization.
 *                          Added/fixed SD card read and write.
 *                          Added FAT32 functionality.
 *                          Added directory search to discover all WAV files.
 *                          Added functions to parse and search WAV files.
 *                          Added functions to play wav file based on file
 *                            number.
 *                          Changed function names to follow new convention.
 *                          Fixed bugs in WAV file playback.
 *                          Increased DAC_FIFO size.
 *                          Added UART module.
 *                          Added PIR functionality.
 *                          Fixed WAV playback bugs. (For best results use a
 *                            fast SD card with 64 k of allocation space with
 *                            a song sampled at 16000Hz mono.)
 *                          Added RGB LED functions.
 *                          Added code to work with the new hardware (PCB_revA).
 *                          Added debug UART, ADC, and timer functionality.
 *                          Added random number generator to randomly choose a
 *                            WAV file to play.
 *                          Added code to read the 5 volt rail.
 *                          Added SD functions to use SD continuous read to
 *                            play WAV files.
 *                          Added debug UART.
 *                          Fixed bugs in random number generator and audio
 *                            playback.
 *                          Added diagnostic to show the user when the SD card
 *                            is formatted properly but all WAV files are
 *                            invalid.
 *                          Print message when there are no valid WAV files.
 *                          Create Banner to display the firmware version and
 *                            hardware version.
/******************************************************************************/

/******************************************************************************/
/* Contains main function.
 *
/******************************************************************************/


/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */
#include <stdio.h>       /* For true/false definition */
#include <string.h>

#include "SYSTEM.h"
#include "USER.h"
#include "MISC.h"
#include "DAC.h"
#include "RTCC.h"
#include "SPI.h"
#include "SD.h"
#include "FAT.h"
#include "PIR.h"
#include "UART.h"
#include "PWM.h"
#include "SWITCH.h"
#include "ADC.h"
#include "TIMERS.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

int main (void)
{
    unsigned char i;
    unsigned char playfile;
    unsigned char SD_State_Old = 255;

    /* Initialize */
    SYS_ConfigureOscillator();
    Init_App();
    Init_System();
    PWM_SetColor(OFF);
    RTCC_SetTime();

    /* Display Banner */
    UART_DisplayBanner();

    /* Play start up message out of debug port */
    UART_DEBUG_SendStringConstCRLN("Starting...");

    /* Display status of PIR sensor */
    if(PIR_STATUS)
    {
        UART_DEBUG_SendStringConstCRLN("PIR sensor initialized");
    }
    else
    {
        UART_DEBUG_SendStringConstCRLN("PIR sensor not working");
    }

    /* Flash LEDS */
    MSC_RedLEDOFF();
    for(i=0; i<20;i++)
    {
        PWM_SetColor(i>>1);
        MSC_RedLEDTOGGLE();
        MSC_DelayUS(50000);
    }
    MSC_RedLEDOFF();

    /* Play start-up song */
    DAC_Play_Startup();
    PWM_SetColor(RED);

    /* Read Voltage rails */
    ADC_ReadVIN();
    ADC_ReadFiveVoltRail();
    
    while(1)
    {
        //RTCC_Read(&CurrentTime);
        
        /* SD card routine */
        if(SD_CardPresent())
        {
            SD_POWER(ON);
            if(SD_State == NOT_INITIALIZED)
            {
                if(SD_State_Old != SD_State)
                {
                    UART_DEBUG_SendStringConstCRLN("Initializing SD Card");
                }                
                InitSD();
            }
            else if(SD_State == INITIALIZED)
            {
                if(SD_State_Old != SD_State)
                {
                    UART_DEBUG_SendStringConstCRLN("Searching the FAT32 partition for WAV files");
                }
                if(InitFAT())
                {
                    UART_DEBUG_SendStringConstCRLN("SD card is correctly formatted FAT32 and contains WAV files");
                    SD_State = WAV_READY;
                }
            }
            else if(SD_State == WAV_READY)
            {
                if(Valid_Wav)
                {
                    /* there are WAV files to be played */ 
                    if(SD_State_Old != SD_State)
                    {
                        UART_DEBUG_SendStringConstCRLN("Ready to play WAV");
                    }
                    PWM_SetColor(GREEN);
                    if(Motion == TRUE || DoorOpened == TRUE)
                    {
                        UART_DEBUG_SendStringConstCRLN("Motion Detected: Selecting random file for playback");
                        playfile = (unsigned char)TMR_RandomNum((long)WaveFilesNumLow,(long)WaveFilesNumHigh);

                        /* Check to see if the file is marked as valid */
                        while(ValidWAVFiles[playfile] != PASS)
                        {
                            if(playfile >= WaveFilesNumHigh)
                            {
                                playfile = 0;
                            }
                            else
                            {
                                playfile++;
                            }
                        }

                        PWM_SetColor(BLUE);
                        PIR_Interrupt(OFF);
                        UART_DEBUG_SendStringConst("Playing WAV file: ");
                        UART_DEBUG_SendStringConstCRLN(&FileList[playfile].name);
                        if(WAV_PlayFile_Random_Sector(playfile))
                        {
                            UART_DEBUG_SendStringConstCRLN("Wav played successfully");
                        }
                        else
                        {
                            UART_DEBUG_SendStringConstCRLN("Wav failed");
                            if(SD_CardPresent() == FAIL)
                            {
                                SD_State = NOT_INITIALIZED;
                                UART_DEBUG_SendStringConstCRLN("SD card removed");
                            }
                        }
                        Motion = FALSE;
                        DoorOpened = FALSE;
                        PIR_Interrupt(ON);
                        PWM_SetColor(GREEN);
                        MSC_RedLEDOFF();
                    }
                }
                else
                {
                    /* there are no satisfactory WAV files on the card */
                    if(SD_State_Old != SD_State)
                    {
                        UART_DEBUG_SendStringConstCRLN(" WAV files are corrupted");
                    }
                    /* Reinitialize */
                    SD_State = NOT_INITIALIZED;
                }
            }
        }
        else
        {
            SD_State = NOT_INITIALIZED;
            if(SD_State_Old != SD_State)
            {
                UART_DEBUG_SendStringConstCRLN("No SD card found");
            }
            
            if(Motion == TRUE || DoorOpened == TRUE)
            {
                PIR_Interrupt(OFF);
                MSC_RedLEDON();
                PWM_SetColor(WHITE);
                MSC_DelayUS(50000);
                Motion = FALSE;
                DoorOpened = FALSE;
                PIR_Interrupt(ON);
            }
            MSC_RedLEDOFF();
            SD_POWER(OFF);
            PWM_SetColor(RED);
        }
        SD_State_Old = SD_State;
    }
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/
