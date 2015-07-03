/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 07/02/15     1.0_DW0a    Initial coding.
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#ifndef PIR_H
#define	PIR_H

#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

/******************************************************************************/
/* PIR_UART_BAUD
 *
 * This is the BAUD rate of the PIR sensor.
/******************************************************************************/
#define PIR_UART_BAUD 4800

/******************************************************************************/
/* PIR_RESPONSE_SIZE
 *
 * This is the BAUD rate of the PIR sensor.
/******************************************************************************/
#define PIR_RESPONSE_SIZE 10

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
extern const unsigned char CONF_SEQUENCE[4];
extern volatile unsigned char Motion;

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Macro Functions                                                            */
/******************************************************************************/

/******************************************************************************/
/* PIR_Asleep
 *
 * The function puts the PIR sensor to sleep.
/******************************************************************************/
#define PIR_Asleep()  (LATA &= ~PIR_SLEEP)

/******************************************************************************/
/* PIR_Awake
 *
 * The function puts the PIR sensor to sleep.
/******************************************************************************/
#define PIR_Awake()  (LATA |= PIR_SLEEP)

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/
inline void PIR_Interrupt(unsigned char status);
void InitPIR(void);
unsigned char PIR_ReadCommand(const unsigned char Command);
unsigned char PIR_WriteCommand(const unsigned char Command, unsigned char* CurrentValue, unsigned char* NewValue);
unsigned char PIR_ConfirmationCommand(const unsigned char Command);
void PIR_Sleep(unsigned char status);
void PIR_Reset(void);

#endif	/* PIR_H */