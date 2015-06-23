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
    unsigned char status;

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
            //SD_readRegister(SD_SetCMD(CMD56, READ));
            for(j=1; j <513;j++)
            {
                Receive_Buffer_Big[j-1] = (unsigned char)(( j >> 3) + 1);
            }
            status = SD_writeBlock(0,Receive_Buffer_Big);
            if(status)
            {
                 SD_readBlock(0);
                 if(Receive_Buffer_Big[1] == 1)
                 {
                     Nop();
                 }
            }
            RedLEDON();
            for(j=0;j<SD.Blocks;j++)
            {
                if(!SD_readBlock(j))
                {
                    SD_Initialized = FALSE;
                    break;
                }
                if(Receive_Buffer_Big[0] != 0)
                {
                    Nop();
                }
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
