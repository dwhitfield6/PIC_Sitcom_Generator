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
    CLKDIVbits.PLLPRE = 0x0; /* N1 = 2 */
    PLLFBDbits.PLLDIV = 30; /* M = 40 */
    CLKDIVbits.PLLPOST = 0x1; /* N2 = 4 */
    /* FOSC = 16MHz * (32/8) = 64 = 32MIPS */
    while(!OSCCONbits.LOCK);
}

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/