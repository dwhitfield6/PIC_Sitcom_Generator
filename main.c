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
    RTCC_SetTime();
    DAC_Play_Startup();
    
    RedLEDOFF();
    for(i=0; i<20;i++)
    {
        RedLEDTOGGLE();
        MSC_DelayUS(50000);
    }
    RedLEDOFF();

    while(1)
    {
        //RTCC_Read(&CurrentTime);
        if(SD_State == NOT_INITIALIZED)
        {
            RedLEDOFF();
            InitSD();
        }
        else if(SD_State == INITIALIZED)
        {
            RedLEDON();
            if(InitFAT())
            {
                SD_State = WAV_READY;
            }
        }
        else if(SD_State == WAV_READY)
        {
            WAV_PlayFile(0);
        }
    }
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/
