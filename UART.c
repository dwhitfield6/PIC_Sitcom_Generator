/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 07/02/15     1.0_DW0a    Initial coding.
 * /******************************************************************************/

/******************************************************************************/
/* Contains Functions for the UART.
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
#include "system.h"
#include "UART.h"
#include "PIR.h"
#include "MISC.h"

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
unsigned char UART_Receive_Buffer[UART_BUFFER_SIZE];
volatile unsigned int UART_Receive_Buffer_Place = 0;
volatile unsigned char RX_Response;

/******************************************************************************/
/* Inline Functions                                                           */
/******************************************************************************/

/******************************************************************************/
/* UART_TX_PIN
 *
 * This function controls the TX pin direction.
/******************************************************************************/
inline void UART_TX_PIN(unsigned char status)
{
    if(status)
    {
        PIR_TX_Tris         = OUTPUT;
        RPOR7bits.RP15R = 0x03;   // RP15 = UART1 Transmit
    }
    else
    {
        PIR_TX_Tris         = INPUT;
    }
}

/******************************************************************************/
/* UART_RX_Interrupt
 *
 * Controls the UART receive interrupt.
/******************************************************************************/
inline void UART_RX_Interrupt(unsigned char status)
{
    if(status)
    {
        IEC0bits.U1RXIE = 1;
    }
    else
    {
        IEC0bits.U1RXIE = 0;
    }
}

/******************************************************************************/
/* Functions
/******************************************************************************/

/******************************************************************************/
/* InitUART
 *
 * The function initializes the UART channel.
/******************************************************************************/
void InitUART(void)
{
    UART_CleanBuffer();
    IPC2bits.U1RXIP = 1; // Interrupt priority is 1
    RPINR18bits.U1RXR = PIR_RX_RP;
    UART_SetClock(PIR_UART_BAUD);
    U1MODEbits.UARTEN = ON;     // Turn on module
    U1STAbits.UTXEN = ON;       // Turn on transmit
    UART_RX_Interrupt(ON);
}

/******************************************************************************/
/* UART_SetClock
 *
 * The function sets the clock speed (Baud rate) of the UART.
/******************************************************************************/
void UART_SetClock(unsigned long baud)
{
    unsigned long div;

    if(U1MODEbits.BRGH)
    {
        div = FCY /(4* baud * 2) - 1;
    }
    else
    {
        div = FCY /(16* baud * 2) - 1;
    }
    U1BRG = div;
}

/******************************************************************************/
/* UART_SendChar
 *
 * The function sends one character of data over the communication port.
/******************************************************************************/
void UART_SendChar(unsigned char data)
{
    U1TXREG = data;
    while(U1STAbits.TRMT);
    while(!U1STAbits.TRMT); //wait for the character to be transmitted.
}

/******************************************************************************/
/* UART_SendCharConst
 *
 * The function sends one constant character of data over the communication
 *  port.
/******************************************************************************/
void UART_SendCharConst(const unsigned char data)
{
    U1TXREG = data;
    while(U1STAbits.TRMT);
    while(!U1STAbits.TRMT); //wait for the character to be transmitted.
}

/******************************************************************************/
/* UART_SendString
 *
 * The function sends one string of characters over the communication port.
/******************************************************************************/
void UART_SendString(unsigned char* data)
{
    while(*data !=0)
    {
        UART_SendChar(*data);
        data++;
    }
}

/******************************************************************************/
/* UART_SendStringConst
 *
 * The function sends one string of constant characters over the communication
 *  port.
/******************************************************************************/
void UART_SendStringConst(const unsigned char* data)
{
    while(*data !=0)
    {
        UART_SendCharConst(*data);
        data++;
    }
}

/******************************************************************************/
/* UART_CleanBuffer
 *
 * The function cleans the UART receive buffer.
/******************************************************************************/
void UART_CleanBuffer(void)
{
    unsigned char status;
    
    status = IEC0bits.U1RXIE;
    UART_RX_Interrupt(OFF);
    UART_Receive_Buffer_Place = 0;
    MSC_CleanBuffer(UART_Receive_Buffer,UART_BUFFER_SIZE);
    if(status)
    {
        UART_RX_Interrupt(ON);
    }
}

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/