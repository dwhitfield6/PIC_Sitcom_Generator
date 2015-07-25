/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 07/24/15     1.0_DW0a    Initial coding.
 * /******************************************************************************/

/******************************************************************************/
/* Contains Functions for the analog to digital converter.
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
#include "SYSTEM.h"
#include "ADC.h"
#include "MISC.h"

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
double VIN = 0.0;

/******************************************************************************/
/* Inline Functions                                                           */
/******************************************************************************/

/******************************************************************************/
/* Functions
/******************************************************************************/

/******************************************************************************/
/* InitTIMERS
 *
 * The function initializes the timers.
/******************************************************************************/
void InitADC(void)
{
#ifndef SitCom_Generator_PROTOBOARD
    ADC_ModuleOFF();
    AD1CON2bits.VCFG = 0x0; // reference voltage is AVDD and AVSS
    AD1CON3bits.ADCS = 0b00111111; // 64 * TCY ~1.6uS
    AD1CON1bits.AD12B = 1; //12 bit, 1 channel
    ADC_ModuleON();
#else
    Nop();
#endif
}

/******************************************************************************/
/* ADC_ReadVIN
 *
 * The function reads the input voltage.
/******************************************************************************/
void ADC_ReadVIN(void)
{
    int RawCounts = 0;
    double Voltage = 0.0;

    AD1CHS0bits.CH0SB = ADC_VIN_AN;
    AD1CHS0bits.CH0SA = ADC_VIN_AN;
    AD1CON1bits.SAMP = TRUE; // sample the input
    MSC_DelayUS(1000);
    AD1CON1bits.SAMP = FALSE; // sample the input
    AD1CON1bits.DONE = 0;
    while(!AD1CON1bits.DONE);
    RawCounts = ADC1BUF0;
    Voltage = ((double) RawCounts * 3.3) / (1<<12);
    VIN = (Voltage * (R10 + R11)) / R11;
}

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/