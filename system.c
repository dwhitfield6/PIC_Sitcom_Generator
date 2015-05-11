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
 * 04/09/15     1.0_DW0b    Fixed bugs.
 *                          Added features.
/******************************************************************************/

/******************************************************************************/
/* Contains system functions.
 *
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#include "system.h"

/******************************************************************************/
/* Functions
/******************************************************************************/

/******************************************************************************/
/* ConfigureOscillator
 *
 * The function waits for the high frequency occilator to be working and stable.
/******************************************************************************/
void ConfigureOscillator(void)
{
    /* this is done in configuration_bits.c so this is not necessary */
    OSCCONbits.COSC = 0x3; // Primary oscilator with PLL
    while(!OSCCONbits.LOCK);
}

void SYSTEM_Initialize (void)
{
    
}

void USER_SdSpiConfigurePins_1 (void)
{
    // Deassert the chip select pin
    LATBbits.LATB1 = 1;
    // Configure CS pin as an output
    TRISBbits.TRISB1 = 0;
    // Configure CD pin as an input
    TRISAbits.TRISA0 = 1;
    // Configure WP pin as an input
    TRISBbits.TRISB1 = 1;
}

inline void USER_SdSpiSetCs_1(uint8_t a)
{
    LATBbits.LATB1 = a;
}

inline bool USER_SdSpiGetCd_1(void)
{
    return (!PORTAbits.RA0) ? true : false;
}

inline bool USER_SdSpiGetWp_1(void)
{
    return (PORTBbits.RB1) ? true : false;
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/