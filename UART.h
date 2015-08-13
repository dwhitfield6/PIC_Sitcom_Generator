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
/* PRINT_COLUMNS
 *
 * This is the amount of columns to print from the debug port.
/******************************************************************************/
#define PRINT_COLUMNS 8

/******************************************************************************/
/* DEBUG_UART_BAUD
 *
 * This is the BAUD rate of the debug interface.
/******************************************************************************/
#define DEBUG_UART_BAUD 115200

/******************************************************************************/
/* UART_BUFFER_SIZE_PIR
 *
 * This is the size of the UART receive buffer associated with the PIR sensor.
/******************************************************************************/
#define UART_BUFFER_SIZE_PIR 10

/******************************************************************************/
/* UART_BUFFER_SIZE_DEBUG
 *
 * This is the size of the UART receive buffer associated with the DEBUG port.
/******************************************************************************/
#define UART_BUFFER_SIZE_DEBUG 10

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
extern unsigned char UART_Rx_Buffer_PIR[UART_BUFFER_SIZE_PIR];
extern unsigned char UART_Rx_Buffer_DEBUG[UART_BUFFER_SIZE_DEBUG];
extern volatile unsigned int UART_Rx_Buffer_Place_PIR;
extern volatile unsigned int UART_Rx_Buffer_Place_DEBUG;
extern volatile unsigned char RX_Response_PIR;
extern volatile unsigned char RX_Response_DEBUG;
extern unsigned char Print_Col;

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
inline void UART_PIR_TX_PIN(unsigned char status);
inline void UART_DEBUG_TX_PIN(unsigned char status);
inline void UART_PIR_RX_Interrupt(unsigned char status);
inline void UART_DEBUG_RX_Interrupt(unsigned char status);
void InitUART(void);
void UART_PIR_SetClock(unsigned long baud);
void UART_DEBUG_SetClock(unsigned long baud);
void UART_PIR_SendChar(unsigned char data);
void UART_DEBUG_SendChar(unsigned char data);
void UART_PIR_SendCharConst(const unsigned char data);
void UART_DEBUG_SendCharConst(const unsigned char data);
void UART_PIR_SendString(unsigned char* data);
void UART_DEBUG_SendString(unsigned char* data);
void UART_PIR_SendStringConst(const unsigned char* data);
void UART_DEBUG_SendStringConst(const unsigned char* data);
void UART_PIR_CleanBuffer(void);
void UART_DEBUG_CleanBuffer(void);
void UART_Display_WAV(int value);
void UART_DEBUG_SendStringConstCRLN(const unsigned char* data);
void UART_DisplayBanner(void);

#endif	/* UART_H */