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
/* Files to Include                                                           */
/******************************************************************************/
#ifndef SYSTEM_H
#define	SYSTEM_H

#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#include "USER.h"

/******************************************************************************/
/* System frequency
 *
 * This is the CPU clock frequency.
 *
 * For this system the clock is 64MHz and the internal PLL is used.
 *
/******************************************************************************/
#ifdef PROTOBOARD
#define FOSC        64000000L
#else
#define FOSC        80000000L
#endif
/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/
#define FCY             (FOSC/2)
#define SYS_FREQ        FCY

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/
void SYS_ConfigureOscillator(void); /* Handles clock switching/osc initialization */

#endif	/* SYSTEM_H */