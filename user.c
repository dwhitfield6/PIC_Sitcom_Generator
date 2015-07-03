/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 04/02/15     1.0_DW0a    Initial project make.
 *                          Derived from project 'PIC_PS2_to_UART'.
 *                          Fixed bugs.
 *                          Added features.
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
#include "SPI.h"
#include "RTCC.h"
#include "SD.h"
#include "UART.h"
#include "PIR.h"

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

/******************************************************************************/
/* Inline Functions
/******************************************************************************/

/******************************************************************************/
/* Functions
/******************************************************************************/

/******************************************************************************/
/* Init_App
 *
 * The function initializes the application. It sets the pins and pull-ups.
/******************************************************************************/
void Init_App(void)
{
    LATA = 0;
    LATB = 0;
    
    ADPCFG              = 0xFFFF; /* All pins are digital */
    RedLEDTris          = OUTPUT;
    AudioAmpMuteTris    = OUTPUT;
    AudioAmpStandbyTris = OUTPUT;
    SOSCOTris           = INPUT;
    SOSCITris           = INPUT;
    SPI_SD_MISOTris     = INPUT;
    SPI_SD_MISO2Tris    = INPUT;
    SPI_SD_MOSITris     = OUTPUT;
    PIR_RX_Tris         = INPUT;
    PIR_TX_Tris         = INPUT;
    PIR_MD_Tris         = INPUT;
    PIR_SLEEP_Tris      = OUTPUT;
}

/******************************************************************************/
/* Init_System
 *
 * The function initializes the system
/******************************************************************************/
void Init_System (void)
{
    InitDAC();
    InitSPI();
    InitSD();
    InitRTCC();
    InitUART();
    InitPIR();
}

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/