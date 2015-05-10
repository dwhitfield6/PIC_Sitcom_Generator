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
#include "ADC.h"


/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

/******************************************************************************/
/* Functions
/******************************************************************************/

/******************************************************************************/
/* InitApp
 *
 * The function initializes the application. It sets the pins and pull-ups.
/******************************************************************************/
void InitApp(void)
{
    LATA = 0;
    LATB = 0;
    
    OrangeLEDTris       = OUTPUT;
    AudioInputTris      = INPUT;
    PotTris             = INPUT;
    AudioAmpMuteTris    = OUTPUT;
    AudioAmpStandbyTris = OUTPUT;

    ACLKCONbits.APSTSCLR = 5; // Auxiliary Clock Output Divider is 4
}

/******************************************************************************/
/* Init_System
 *
 * The function initializes the system
/******************************************************************************/
void Init_System (void)
{
    InitADC();
    InitDAC();
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/