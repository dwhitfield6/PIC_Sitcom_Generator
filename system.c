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
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/