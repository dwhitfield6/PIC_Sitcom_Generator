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
 *                          Fixed WAV playback bugs. (MFor best results use a
 *                            fast sd card with 64 k of allocation space with
 *                            a song sampled at 16000Hz mono.)
 *                          Added RGB LED functions.
 *                          Added code to work withteh new hardware (PCB_revA)
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

#include "system.h"
#include "user.h"
#include "MISC.h"
#include "DAC.h"
#include "RTCC.h"
#include "SPI.h"
#include "SD.h"
#include "FAT.h"
#include "PIR.h"
#include "UART.h"
#include "PWM.h"

/******************************************************************************/
/* Version number                                                             */
/******************************************************************************/

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

    SYS_ConfigureOscillator();
    Init_App();
    Init_System();
    PWM_SetColor(OFF);
    RTCC_SetTime();

    RedLEDOFF();
    for(i=0; i<20;i++)
    {
        PWM_SetColor(i>>1);
        RedLEDTOGGLE();
        MSC_DelayUS(50000);
    }
    RedLEDOFF();

    DAC_Play_Startup();
    PWM_SetColor(RED);

    while(1)
    {
        //RTCC_Read(&CurrentTime);
        
        /* SD card routine */
        if(SD_CardPresent())
        {
            if(SD_Card_Status_Prev != SD_Card_Status)
            {
                /* SD card was just inserted */
                SD_POWER(ON);
            }
            if(SD_State == NOT_INITIALIZED)
            {
                InitSD();
            }
            else if(SD_State == INITIALIZED)
            {
                if(InitFAT())
                {
                    SD_State = WAV_READY;
                }
            }
            else if(SD_State == WAV_READY)
            {
                PWM_SetColor(GREEN);
                WAV_PlayFile(0);
                if(Motion == TRUE)
                {
                    PWM_SetColor(PURPLE);
                    PIR_Interrupt(OFF);
                    WAV_PlayFile(3);
                    Motion = FALSE;
                    PIR_Interrupt(ON);
                    PWM_SetColor(GREEN);
                    RedLEDOFF();
                }
            }
        }
        else
        {
            SD_POWER(OFF);
            PWM_SetColor(RED);
        }
        SD_Card_Status_Prev = SD_Card_Status;
    }
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/
