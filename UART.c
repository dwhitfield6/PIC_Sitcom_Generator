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

#include "USER.h"
#include "SYSTEM.h"
#include "UART.h"
#include "PIR.h"
#include "MISC.h"

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
unsigned char UART_Rx_Buffer_PIR[UART_BUFFER_SIZE_PIR];
unsigned char UART_Rx_Buffer_DEBUG[UART_BUFFER_SIZE_DEBUG];
volatile unsigned int UART_Rx_Buffer_Place_PIR = 0;
volatile unsigned int UART_Rx_Buffer_Place_DEBUG = 0;
volatile unsigned char RX_Response_PIR;
volatile unsigned char RX_Response_DEBUG;
unsigned char Print_Col = 0;

/******************************************************************************/
/* Inline Functions                                                           */
/******************************************************************************/

/******************************************************************************/
/* UART_PIR_TX_PIN
 *
 * This function controls the TX pin direction associated with the PIR sensor.
/******************************************************************************/
inline void UART_PIR_TX_PIN(unsigned char status)
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
/* UART_DEBUG_TX_PIN
 *
 * This function controls the TX pin direction associated with the debug port.
/******************************************************************************/
inline void UART_DEBUG_TX_PIN(unsigned char status)
{
#ifndef PROTOBOARD
    if(status)
    {
        DBG_TX_Tris         = OUTPUT;
        RPOR1bits.RP3R      = 0x05;   // RP3 = UART2 Transmit
    }
    else
    {
        DBG_TX_Tris         = INPUT;
    }
#endif
}

/******************************************************************************/
/* UART_PIR_RX_Interrupt
 *
 * Controls the UART receive interrupt associated with the PIR sensor.
/******************************************************************************/
inline void UART_PIR_RX_Interrupt(unsigned char status)
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
/* UART_DEBUG_RX_Interrupt
 *
 * Controls the UART receive interrupt associated with the debug port.
/******************************************************************************/
inline void UART_DEBUG_RX_Interrupt(unsigned char status)
{
    if(status)
    {
        IEC1bits.U2RXIE = 1;
    }
    else
    {
        IEC1bits.U2RXIE = 0;
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
    UART_PIR_CleanBuffer();
    IPC2bits.U1RXIP = 1; // Interrupt priority is 1
    RPINR18bits.U1RXR = PIR_RX_RP;
    UART_PIR_SetClock(PIR_UART_BAUD);
    U1MODEbits.UARTEN = ON;     // Turn on module
    U1STAbits.UTXEN = ON;       // Turn on transmit
    UART_PIR_RX_Interrupt(ON);

#ifndef PROTOBOARD
    UART_DEBUG_CleanBuffer();
    IPC7bits.U2RXIP = 1; // Interrupt priority is 1
    RPINR19bits.U2RXR = DBG_RX_RP;
    UART_DEBUG_SetClock(DEBUG_UART_BAUD);
    U2MODEbits.UARTEN = ON;     // Turn on module
    U2STAbits.UTXEN = ON;       // Turn on transmit
    UART_DEBUG_RX_Interrupt(ON);
    UART_DEBUG_TX_PIN(ON);
#endif
}

/******************************************************************************/
/* UART_PIR_SetClock
 *
 * The function sets the clock speed (Baud rate) of the UART associated with
 *  the PIR sensor.
/******************************************************************************/
void UART_PIR_SetClock(unsigned long baud)
{
    unsigned long div;

    if(U1MODEbits.BRGH)
    {
        div = FCY /(4* baud) - 1;
    }
    else
    {
        div = FCY /(16* baud) - 1;
    }
    U1BRG = div;
}

/******************************************************************************/
/* UART_DEBUG_SetClock
 *
 * The function sets the clock speed (Baud rate) of the UART associated with
 *  the debug port.
/******************************************************************************/
void UART_DEBUG_SetClock(unsigned long baud)
{
    unsigned long div;

    if(U2MODEbits.BRGH)
    {
        div = FCY /(4* baud) - 1;
    }
    else
    {
        div = FCY /(16* baud) - 1;
    }
    U2BRG = div;
}

/******************************************************************************/
/* UART_PIR_SendChar
 *
 * The function sends one character of data over the PIR communication port.
/******************************************************************************/
void UART_PIR_SendChar(unsigned char data)
{
    U1TXREG = data;
    while(U1STAbits.TRMT);
    while(!U1STAbits.TRMT); //wait for the character to be transmitted.
}

/******************************************************************************/
/* UART_DEBUG_SendChar
 *
 * The function sends one character of data over the debug communication port.
/******************************************************************************/
void UART_DEBUG_SendChar(unsigned char data)
{
#ifndef PROTOBOARD
    U2TXREG = data;
    while(U2STAbits.TRMT);
    while(!U2STAbits.TRMT); //wait for the character to be transmitted.
#else
    Nop();
#endif
}

/******************************************************************************/
/* UART_PIR_SendCharConst
 *
 * The function sends one constant character of data over the PIR communication
 *  port.
/******************************************************************************/
void UART_PIR_SendCharConst(const unsigned char data)
{
    U1TXREG = data;
    while(U1STAbits.TRMT);
    while(!U1STAbits.TRMT); //wait for the character to be transmitted.
}

/******************************************************************************/
/* UART_DEBUG_SendCharConst
 *
 * The function sends one constant character of data over the debug
 * communication port.
/******************************************************************************/
void UART_DEBUG_SendCharConst(const unsigned char data)
{
#ifndef PROTOBOARD
    U2TXREG = data;
    while(U2STAbits.TRMT);
    while(!U2STAbits.TRMT); //wait for the character to be transmitted.
#else
    Nop();
#endif
}

/******************************************************************************/
/* UART_PIR_SendString
 *
 * The function sends one string of characters over the PIR communication port.
/******************************************************************************/
void UART_PIR_SendString(unsigned char* data)
{
    while(*data !=0)
    {
        UART_PIR_SendChar(*data);
        data++;
    }
}

/******************************************************************************/
/* UART_DEBUG_SendString
 *
 * The function sends one string of characters over the debug communication
 *  port.
/******************************************************************************/
void UART_DEBUG_SendString(unsigned char* data)
{
    while(*data !=0)
    {
        UART_DEBUG_SendChar(*data);
        data++;
    }
}

/******************************************************************************/
/* UART_PIR_SendStringConst
 *
 * The function sends one string of constant characters over the PIR 
 *  communication port.
/******************************************************************************/
void UART_PIR_SendStringConst(const unsigned char* data)
{
    while(*data !=0)
    {
        UART_PIR_SendCharConst(*data);
        data++;
    }
}

/******************************************************************************/
/* UART_DEBUG_SendStringConst
 *
 * The function sends one string of constant characters over the debug
 *  communication port.
/******************************************************************************/
void UART_DEBUG_SendStringConst(const unsigned char* data)
{
    while(*data !=0)
    {
        UART_DEBUG_SendCharConst(*data);
        data++;
    }
}

/******************************************************************************/
/* UART_DEBUG_SendStringConstCRLN
 *
 * The function sends one string of constant characters over the debug
 *  communication port and send a new line carriage return.
/******************************************************************************/
void UART_DEBUG_SendStringConstCRLN(const unsigned char* data)
{
    while(*data !=0)
    {
        UART_DEBUG_SendCharConst(*data);
        data++;
    }
    UART_DEBUG_SendCharConst('\r');
    UART_DEBUG_SendCharConst('\n');
}

/******************************************************************************/
/* UART_PIR_CleanBuffer
 *
 * The function cleans the UART receive buffer.
/******************************************************************************/
void UART_PIR_CleanBuffer(void)
{
    unsigned char status;
    
    status = IEC0bits.U1RXIE;
    UART_PIR_RX_Interrupt(OFF);
    UART_Rx_Buffer_Place_PIR = 0;
    MSC_CleanBuffer(UART_Rx_Buffer_PIR,UART_BUFFER_SIZE_PIR);
    if(status)
    {
        UART_PIR_RX_Interrupt(ON);
    }
}

/******************************************************************************/
/* UART_DEBUG_CleanBuffer
 *
 * The function cleans the UART receive buffer.
/******************************************************************************/
void UART_DEBUG_CleanBuffer(void)
{
    unsigned char status;

    status = IEC1bits.U2RXIE;
    UART_DEBUG_RX_Interrupt(OFF);
    UART_Rx_Buffer_Place_DEBUG = 0;
    MSC_CleanBuffer(UART_Rx_Buffer_DEBUG,UART_BUFFER_SIZE_DEBUG);
    if(status)
    {
        UART_DEBUG_RX_Interrupt(ON);
    }
}

/******************************************************************************/
/* UART_DEBUG_TestSpeed
 *
 * The function tests the baud rate of the debug UART.
/******************************************************************************/
void UART_DEBUG_TestSpeed(void)
{
    while(1)
    {
        UART_DEBUG_SendCharConst('U');
    }
}

/******************************************************************************/
/* UART_Display_WAV
 *
 * The function prints out the contents of the wav file over the debug port.
/******************************************************************************/
void UART_Display_WAV(int value)
{
    unsigned char buf[10];
    
    MSC_CleanBuffer(buf, 10);

    sprintf(buf,"0x%04X ",value);
    UART_DEBUG_SendString(buf);
    Print_Col++;
    if(Print_Col >= PRINT_COLUMNS)
    {
        Print_Col = 0;
        UART_DEBUG_SendStringConst("\r\n");
    }
}

/******************************************************************************/
/* UART_DisplayBanner
 *
 * The function prints out the banner.
/******************************************************************************/
void UART_DisplayBanner(void)
{

    UART_DEBUG_SendStringConstCRLN("Sitcom Generator");
    UART_DEBUG_SendStringConst("Hardware Version: ");
    UART_DEBUG_SendStringConstCRLN(PCB_Version);
    UART_DEBUG_SendStringConst("Firmware Version: ");
    UART_DEBUG_SendStringConstCRLN(FW_Version);
    UART_DEBUG_SendStringConstCRLN("Copywrite by Marley Circuits\r\n");
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/