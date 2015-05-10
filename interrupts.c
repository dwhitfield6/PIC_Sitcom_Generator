/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 04/02/15     1.0_DW0a    Initial project make.
 *                          Derived from project 'PIC_Smart_Rf'.
 * 04/09/15     1.0_DW0b    Fixed bugs.
 *                          Added features.
/******************************************************************************/

/******************************************************************************/
/* Contains ISR
 *
/******************************************************************************/
extern unsigned int AudioCounts;
extern unsigned char Volume;
extern unsigned int Audio;
unsigned char array = 1;
/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include <xc.h>        /* XC8 General Include File */

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#include "MISC.h"
#include "Sound.h"
#include "Sound_Laugh.h"


/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/

#define _ISR_PSV __attribute__((__interrupt__, __auto_psv__))
unsigned long place =0;

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/
void _ISR_PSV _DAC1RInterrupt(void)
{
    IFS4bits.DAC1RIF = 0; /* Clear Right Channel Interrupt Flag */
    if(array == 1)
    {
        DAC1RDAT = (data[place] << Volume); /* User Code to Write to FIFO Goes Here */
    }
    else if(array == 2)
    {
        DAC1RDAT = (data2[place] << Volume); /* User Code to Write to FIFO Goes Here */
    }
    else
    {
        DAC1RDAT = (data3[place] << Volume); /* User Code to Write to FIFO Goes Here */
    }

    place++;
    if(place > NUM_ELEMENTS)
    {
        place =0;
        array++;
        if(array >= 4)
        {
            array = 1;
        }
    }
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/