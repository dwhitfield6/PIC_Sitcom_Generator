/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 04/02/15     1.0_DW0     Initial project make.
/******************************************************************************/

/******************************************************************************/
/* Contains functions that control the internal ADC.
 *
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include <xc.h>        /* XC8 General Include File */

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#include "ADC.h"        
#include "user.h"          
#include "MISC.h"          

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/

/******************************************************************************/
/* ReadAudio
 *
 * The function returns the Audio signal on the 3.5mm stereo plug.
/******************************************************************************/
unsigned int ReadAudio(void)
{
    unsigned int temp;
    InternalADC_Read(AN1>>1,&temp);
    return temp;
}

/******************************************************************************/
/* ReadPot
 *
 * The function returns the AC counts on the pot.
/******************************************************************************/
unsigned int ReadPot(void)
{
    unsigned int temp;
    InternalADC_Read(AN0>>1,&temp);
    return temp;
}

/******************************************************************************/
/* InternalADC_Read
 *
 * The function returns the digital adc counts of the channel.
/******************************************************************************/
unsigned char InternalADC_Read(unsigned char channel, unsigned int *ADCcounts)
{
    AD1CON1bits.SAMP =1; // Start sampling
    AD1CHS0bits.CH0SA = channel;
    AD1CHS0bits.CH0SB = channel;
    AD1CON1bits.DONE =0; // Start new reading
    AD1CON1bits.SAMP =0; // Start sampling
    while(!AD1CON1bits.DONE);
    *ADCcounts = ADC1BUF0;


    return OK;
}

/******************************************************************************/
/* ResetADC
 *
 * The function shutsdown the ADC module and sets all registers to their
 *   preinitialized values.
/******************************************************************************/
void ResetADC(void)
{
    AD1CON1 = 0;
    AD1CON2 = 0;
    AD1CON3 = 0;
}

/******************************************************************************/
/* EnableADC
 *
 * The function Enables the ADC module.
/******************************************************************************/
void EnableADC(void)
{
    PMD1bits.AD1MD = 0;
}


/******************************************************************************/
/* InitADC
 *
 * This function initializes the ADC.
/******************************************************************************/
void InitADC(void)
{
    ResetADC();
    EnableADC();
    AD1PCFGL = 0xFFF; //All digital
    AD1PCFGL &= ~(AN0 + AN1); // AN0 and AN1 are used
    AD1CON2bits.VCFG = 0; // Reference is AVSS and AVDD
    AD1CON3bits.ADRC = 0; // Use system clock
    AD1CON3bits.ADCS = 0x0; // Tad = Tcy
    AD1CON2bits.CHPS = 0; // Channel 0
    AD1CON1bits.SSRC = 0; // Clearing sample bit ends sampling and starts conversion
    AD1CON1bits.AD12B = 1; // 12 bit mode
    AD1CON1bits.FORM = 0x00; // 00 = Integer (DOUT = 0000 dddd dddd dddd)
    AD1CON1bits.ADON = 1; // ADC module is operating
}

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/