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
/* Files to Include                                                           */
/******************************************************************************/
#ifndef SYSTEM_H
#define	SYSTEM_H

#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

/******************************************************************************/
/* System frequency
 *
 * This is the CPU clock frequency.
 *
 * For this system the clock is 32MHz and the internal PLL is used.
 *
/******************************************************************************/

#define SYS_FREQ        64000000L

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

#define FCY             SYS_FREQ/4



// Definition for system clock
#define SYS_CLK_FrequencySystemGet()           32000000
// Definition for peripheral clock
#define SYS_CLK_FrequencyPeripheralGet()           SYS_CLK_FrequencySystemGet()
// Definition for instruction clock
#define SYS_CLK_FrequencyInstructionGet()          (SYS_CLK_FrequencySystemGet() / 2)


// System initialization function
void SYSTEM_Initialize (void);

// User-defined function to set the chip select for our example drive
void USER_SdSpiSetCs_1 (uint8_t a);
// User-defined function to get the card detection status for our example drive
bool USER_SdSpiGetCd_1 (void);
// User-defined function to get the write-protect status for our example drive
bool USER_SdSpiGetWp_1 (void);
// User-defined function to initialize tristate bits for CS, CD, and WP
void USER_SdSpiConfigurePins_1 (void);


/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/

void ConfigureOscillator(void); /* Handles clock switching/osc initialization */


#endif	/* SYSTEM_H */