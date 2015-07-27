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

#include "USER.h"
#include "MISC.h"
#include "DAC.h"
#include "SPI.h"
#include "RTCC.h"
#include "SD.h"
#include "UART.h"
#include "PIR.h"
#include "PWM.h"
#include "SWITCH.h"
#include "TIMERS.h"
#include "ADC.h"

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
    
    AD1PCFGL              = 0xFFFF; /* All pins are digital */
    RedLEDTris          = OUTPUT;
#ifdef SitCom_Generator_PROTOBOARD
    AudioAmpMuteTris    = OUTPUT;
#else
    SD_PowerTris        = OUTPUT;
    SD_INSERT_Tris      = INPUT;
    RGB_RedTris         = OUTPUT;
    RGB_GreenTris       = OUTPUT;
    RGB_BlueTris        = OUTPUT;
    AudioAmpDiagTris    = INPUT;
    TP1_Tris            = OUTPUT;
    TP2_Tris            = OUTPUT;
    TP3_Tris            = OUTPUT;
    TP4_Tris            = OUTPUT;
    ADC_VINTris         = INPUT;
    ADC_5INTris         = INPUT;
    AD1PCFGL            &= ~((1L<<ADC_VIN_AN) + (1L<<ADC_5IN_AN)); /* Configure ADC pins as analog */
    PIR_MD2_Tris        = INPUT;
    DBG_TX_Tris         = OUTPUT;
    DBG_RX_Tris         = INPUT;
    DOOR_SW_Tris        = INPUT;
#endif
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
    if(SD_CardPresent())
    {
        SD_POWER(ON);
        MSC_DelayUS(1000);
        InitSD();
    }
    InitSWITCH();
    InitRTCC();
    InitUART();
    InitPIR();
    InitTIMERS();
    InitPWM();
    InitADC();
}

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/