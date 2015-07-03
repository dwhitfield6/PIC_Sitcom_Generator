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
#ifndef UART_H
#define	UART_H

#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

/******************************************************************************/
/* UART_BUFFER_SIZE
 *
 * This is the size of the UART receive buffer.
/******************************************************************************/
#define UART_BUFFER_SIZE 10

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
extern unsigned char UART_Receive_Buffer[UART_BUFFER_SIZE];
extern volatile unsigned int UART_Receive_Buffer_Place;
extern volatile unsigned char RX_Response;

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/
#define ACK     0x06

/******************************************************************************/
/* Macro Functions                                                            */
/******************************************************************************/

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/
inline void UART_TX_PIN(unsigned char status);
inline void UART_RX_Interrupt(unsigned char status);
void InitUART(void);
void UART_SetClock(unsigned long baud);
void UART_SendChar(unsigned char data);
void UART_SendCharConst(const unsigned char data);
void UART_SendString(unsigned char* data);
void UART_SendStringConst(const unsigned char* data);
void UART_CleanBuffer(void);

#endif	/* UART_H */