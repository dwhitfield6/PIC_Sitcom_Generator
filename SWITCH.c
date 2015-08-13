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
/* Contains Functions for switches and to detect misc connections.
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
#include "SWITCH.h"

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
unsigned char DoorOpened = FALSE;

/******************************************************************************/
/* Inline Functions                                                           */
/******************************************************************************/
/******************************************************************************/
/* Door_Interrupt
 *
 * This function controls the external interrupt INT2.
/******************************************************************************/
inline void Door_Interrupt(unsigned char status)
{
    if(status)
    {
        IEC1bits.INT2IE = ON;
    }
    else
    {
        IEC1bits.INT2IE = OFF;
    }
}

/******************************************************************************/
/* Functions
/******************************************************************************/

/******************************************************************************/
/* InitSWITCH
 *
 * The function initializes the door switch.
/******************************************************************************/
void InitSWITCH(void)
{
#ifndef PROTOBOARD
    /* 
     * The Door Switch indicating door has been opened is triggered by INT2.
     * If the door switch is N/O (when the door is closed the the connection
     * is established) the interrupt needs to be triggered by a positive edge,
     * otherwise a negative edge.
     */
    INTCON2bits.INT2EP = 0; // trigger on positive edge
    IPC7bits.INT2IP = 2; // priority is 2
    RPINR1bits.INT2R = DOOR_SW_RP;
    
#else
    Nop();
#endif
}

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/