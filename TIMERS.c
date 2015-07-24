/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 07/22/15     1.0_DW0a    Initial coding.
 * /******************************************************************************/

/******************************************************************************/
/* Contains Functions for the timers.
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
#include "TIMERS.h"

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

/******************************************************************************/
/* Inline Functions                                                           */
/******************************************************************************/

/******************************************************************************/
/* TMR_EnableTimer2
 *
 * Controls the Timer2 module.
/******************************************************************************/
inline void TMR_EnableTimer2(unsigned char action)
{
    if (action)
    {
        T2CONbits.TON = 1; // Enable Timer
    }
    else
    {
        T2CONbits.TON = 0; // Disable Timer
    }
}

/******************************************************************************/
/* Functions
/******************************************************************************/

/******************************************************************************/
/* InitTIMERS
 *
 * The function initializes the timers.
/******************************************************************************/
void InitTIMERS(void)
{
#ifndef SitCom_Generator_PROTOBOARD
    InitTIMER2();
#else
    Nop();
#endif
}

/******************************************************************************/
/* InitTIMER2
 *
 * The function initializes timer 2 which is used for the PWM module.
/******************************************************************************/
void InitTIMER2(void)
{
    TMR_EnableTimer2(OFF);
    T2CONbits.TCS = 0; // Select internal instruction cycle clock
    T2CONbits.TGATE = 0; // Disable Gated Timer mode
    T2CONbits.TCKPS = 0b10; // Select 1:64 Prescaler
    TMR2 = 0x00; // Clear timer register
    PR2 = 1000; // Load the period value
    IPC1bits.T2IP = 0x01; // Set Timer 2 Interrupt Priority Level
    IFS0bits.T2IF = 0; // Clear Timer 2 Interrupt Flag
    IEC0bits.T2IE = 1; // Enable Timer 2 interrupt
    TMR_EnableTimer2(ON);
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/