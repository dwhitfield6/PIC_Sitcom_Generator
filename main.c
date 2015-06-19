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
    long j;

    ConfigureOscillator();
    InitApp();
    Init_System();
    RTCSetTime();
    Play_Startup();
    
    RedLEDOFF();
    for(i=0; i<20;i++)
    {
        RedLEDTOGGLE();
        delayUS(50000);
    }
    RedLEDOFF();

    while(1)
    {
        //RTCread(&CurrentTime);
        if(SD_Initialized)
        {
            RedLEDON();
            for(j=0;j<SD_CardBlocks;j++)
            {
                SD_readBlock(i);
                Nop();
            }
        }
        else
        {
            RedLEDOFF();
            InitSD();
        }
    }
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/
