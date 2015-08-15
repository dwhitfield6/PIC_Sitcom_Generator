/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 08/14/15     1.1_DW0a    Initial coding.
/******************************************************************************/

/******************************************************************************/
/* Contains Functions for the direct memory access controller.
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
#include "DMA.h"
#include "MISC.h"

/******************************************************************************/
/* DMA variables                                                              */
/******************************************************************************/

/************* DMA for RGB LED PWM ***********/
unsigned int DMA_RedDuty __attribute__((space(dma))) = 0;
unsigned int DMA_GreenDuty __attribute__((space(dma))) = 0;
unsigned int DMA_BlueDuty __attribute__((space(dma))) = 0;

/******************************************************************************/
/* Inline Functions                                                           */
/******************************************************************************/

/******************************************************************************/
/* Functions
/******************************************************************************/

/******************************************************************************/
/* DMA_StartRGB
 *
 * The function initializes the DMA channels for the RGB LED.
/******************************************************************************/
void DMA_StartRGB(void)
{
    DMA_StartCH0();
    DMA_StartCH1();
    DMA_StartCH2();
}

/******************************************************************************/
/* DMA_StartCH0
 *
 * The function starts DMA channel 0.
/******************************************************************************/
void DMA_StartCH0(void)
{
    IFS0bits.DMA0IF = 0; // Clear the DMA interrupt flag
    IEC0bits.DMA0IE = 1; // Enable DMA interrupt
    DMA0CONbits.CHEN = 1; // Enable DMA channel
}

/******************************************************************************/
/* DMA_StartCH1
 *
 * The function starts DMA channel 1.
/******************************************************************************/
void DMA_StartCH1(void)
{
    IFS0bits.DMA1IF = 0; // Clear the DMA interrupt flag
    IEC0bits.DMA1IE = 1; // Enable DMA interrupt
    DMA1CONbits.CHEN = 1; // Enable DMA channel
}


/******************************************************************************/
/* DMA_StartCH2
 *
 * The function starts DMA channel 2.
/******************************************************************************/
void DMA_StartCH2(void)
{
    IFS1bits.DMA2IF = 0; // Clear the DMA interrupt flag
    IEC1bits.DMA2IE = 1; // Enable DMA interrupt
    DMA2CONbits.CHEN = 1; // Enable DMA channel
}

/******************************************************************************/
/* DMA_Channel0
 *
 * The function sets up channel 0 of the DMA. This is associated with the OC1RS
 *  register that is used for the Red LED.
/******************************************************************************/
void DMA_Channel0(void)
{
    DMA0CONbits.AMODE = 0b00;   // Register indirect with post increment
    DMA0CONbits.MODE = 0b00;    // Continuous, Ping-Pong mode Disabled
    DMA0CONbits.DIR = 0;        // Peripheral to RAM
    DMA0PAD = (int)&OC1RS;      // Address of the secondary output compare register
    DMA0REQ = 7;                // Select Timer2 interrupt as DMA request source
    DMA0CNT = 1;              // Number of words to buffer.
    DMA0STA = __builtin_dmaoffset(&DMA_RedDuty);
}

/******************************************************************************/
/* DMA_Channel1
 *
 * The function sets up channel 1 of the DMA. This is associated with the OC2RS
 *  register that is used for the Green LED.
/******************************************************************************/
void DMA_Channel1(void)
{
    DMA1CONbits.AMODE = 0b00;   // Register indirect with post increment
    DMA1CONbits.MODE = 0b00;    // Continuous, Ping-Pong mode Disabled
    DMA1CONbits.DIR = 0;        // Peripheral to RAM
    DMA1PAD = (int)&OC2RS;      // Address of the secondary output compare register
    DMA1REQ = 7;                // Select Timer2 interrupt as DMA request source
    DMA1CNT = 1;              // Number of words to buffer.
    DMA1STA = __builtin_dmaoffset(&DMA_GreenDuty);
}

/******************************************************************************/
/* DMA_Channel2
 *
 * The function sets up channel 2 of the DMA. This is associated with the OC3RS
 *  register that is used for the Blue LED.
/******************************************************************************/
void DMA_Channel2(void)
{
    DMA2CONbits.AMODE = 0b00;   // Register indirect with post increment
    DMA2CONbits.MODE = 0b00;    // Continuous, Ping-Pong mode Disabled
    DMA2CONbits.DIR = 0;        // Peripheral to RAM
    DMA2PAD = (int)&OC3RS;      // Address of the secondary output compare register
    DMA2REQ = 7;                // Select Timer2 interrupt as DMA request source
    DMA2CNT = 1;              // Number of words to buffer.
    DMA2STA = __builtin_dmaoffset(&DMA_BlueDuty);
}

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/