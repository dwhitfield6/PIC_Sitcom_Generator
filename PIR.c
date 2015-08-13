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
/* Contains Functions for the Zmotion detection module II.
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
#include "PIR.h"
#include "UART.h"
#include "MISC.h"

/******************************************************************************/
/* Confirmation Sequence                                                      */
/******************************************************************************/
const unsigned char CONF_SEQUENCE[] = { 1,2,3,4,0 };
unsigned char PIR_Init_attempts = 0;
unsigned char PIR_STATUS = FAIL;

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
volatile unsigned char Motion = FALSE;

/******************************************************************************/
/* Inline Functions                                                           */
/******************************************************************************/

/******************************************************************************/
/* PIR_MD_PinControl
 *
 * This function controls the MD/RST pin. The pin is set as an active low reset
 * by default. If theis funtion is TRUE, the pin is set as MD (an input)
 * otherwise it is set as an output as pin RST.
/******************************************************************************/
inline void PIR_MD_PinControl(unsigned char MD_state)
{
    if(MD_state)
    {
        PIR_MD_Tris         = INPUT;
    }
    else
    {
        PIR_MD_Tris         = OUTPUT; //pin is RST not MD
    }
}
/******************************************************************************/
/* PIR_Interrupt
 *
 * This function controls the external interrupt INT1 for pcb versions and the
 * change input interrupt for proto version.
/******************************************************************************/
inline void PIR_Interrupt(unsigned char status)
{
#ifndef PROTOBOARD
    if(status)
    {
        IFS1bits.INT1IF = 0; // clear flag
        IEC1bits.INT1IE = ON;
    }
    else
    {
        IEC1bits.INT1IE = OFF;
    }
#else
    if(status)
    {
        IEC1bits.CNIE = 1;
        CNEN1 |= PIR_MD_CN;
    }
    else
    {
        IEC1bits.CNIE = 0;
        CNEN1 &= ~PIR_MD_CN;
    }
#endif
}

/******************************************************************************/
/* PIR_Interrupt
 *
 * This function controls the external interrupt INT1.
/******************************************************************************/
inline unsigned char PIR_MD_READ(void)
{
#ifndef PROTOBOARD
    if(PORTA & PIR_MD2)
    {
        return TRUE;
    }
    return FALSE;
#else
    return FALSE;
#endif
}

/******************************************************************************/
/* Functions
/******************************************************************************/

/******************************************************************************/
/* InitPIR
 *
 * The function initializes the Zmotion II PIR sensor.
/******************************************************************************/
void InitPIR(void)
{
    unsigned char Receivebuf[PIR_RESPONSE_SIZE];
    unsigned char Sendbuf[PIR_RESPONSE_SIZE];
    MSC_CleanBuffer(Receivebuf, PIR_RESPONSE_SIZE);

#ifndef PROTOBOARD
    /* the PIR module MD pin indicating motion is triggered by INT1 */
    INTCON2bits.INT1EP = 1; // trigger on negative edge
    IPC5bits.INT1IP = 2; // priority is 2
    RPINR0bits.INT1R = PIR_MD_RP;
    PIR_Init_attempts = 0;
    /* disable hard reset until the pin is switched to MD function */

    Initialize:
    PIR_Init_attempts++;
    if(PIR_Init_attempts > PIR_INIT_LIMIT)
    {
        PIR_STATUS = FAIL;
        PIR_Interrupt(OFF);
        return;
    }
    PIR_Reset();        // Put the device into serial interface mode
    PIR_MD_PinControl(FALSE);
    LATC |= PIR_MD;
    MSC_DelayUS(1000); // Wait for data to be received
    PIR_Reset();        // Put the device into serial interface mode
    MSC_DelayUS(1000); // Wait for data to be received

    /* configure MD pin */
    PIR_WriteCommand('C', Receivebuf, "M", NULL);
    PIR_MD_PinControl(TRUE);
    MSC_CleanBuffer(Receivebuf, 10);
    
    /* configure MD Activation time */
    Sendbuf[0] = 1;
    PIR_WriteCommand('D', Receivebuf, Sendbuf, 1);
    MSC_CleanBuffer(Receivebuf, 10);

    /* Disable Hyper Sense */
    Sendbuf[0] = 1;
    PIR_WriteCommand('E', Receivebuf, "N", NULL);
    MSC_CleanBuffer(Receivebuf, 10);

    /* Frequency Response to detect low/high frequencies */
    Sendbuf[0] = 1;
    PIR_WriteCommand('F', Receivebuf, "L", NULL);
    MSC_CleanBuffer(Receivebuf, 10);

    /* configure Range */
    Sendbuf[0] = 3; // range is 0-7 with 0 being most sensitive
    PIR_WriteCommand('R', Receivebuf, Sendbuf, 1);
    MSC_CleanBuffer(Receivebuf, 10);

    /* configure Sensitivity */
    Sendbuf[0] = 100; // range is 0-255 with 0 being most sensitive
    if(!PIR_WriteCommand('S', Receivebuf, Sendbuf, 1))
    {
            MSC_CleanBuffer(Receivebuf, 10);
            goto Initialize;
    }
    PIR_STATUS = PASS;
    MSC_CleanBuffer(Receivebuf, 10);

#else
    IPC4bits.CNIP = 2;  // Interrupt priority is 2
#endif
    Motion = FALSE;
    PIR_Interrupt(ON);
}

/******************************************************************************/
/* PIR_ReadCommand
 *
 * The function sends a read command.
/******************************************************************************/
unsigned char PIR_ReadCommand(const char Command)
{
    unsigned int count = 0;

    UART_PIR_CleanBuffer();
    RX_Response_PIR = FALSE;
    UART_PIR_SendCharConst(Command);
    while(!RX_Response_PIR)
    {
        count++;
        MSC_DelayUS(20); // Wait for data to be received
        if(count > 500)
        {
            return FALSE;
        }
    }
    return TRUE;
}

/******************************************************************************/
/* PIR_WriteCommand
 *
 * The function sends a write command.
/******************************************************************************/
unsigned char PIR_WriteCommand(const char Command, unsigned char* CurrentValue, char* NewValue, unsigned char AmountBytes)
{
    unsigned char i = 0;
    unsigned int count = 0;
    
    if(!PIR_ReadCommand(Command))
    {
        return FALSE;
    }
    Nop();
    MSC_BufferCopy(UART_Rx_Buffer_PIR,CurrentValue, PIR_RESPONSE_SIZE, 0);
    UART_PIR_CleanBuffer();
    RX_Response_PIR = FALSE;
    if(!AmountBytes)
    {
        UART_PIR_SendString(NewValue);
    }
    else
    {
        while(AmountBytes)
        {
            UART_PIR_SendChar(NewValue[i]);
            i++;
            AmountBytes--;
        }
    }
    while(!RX_Response_PIR)
    {
        count++;
        MSC_DelayUS(20); // Wait for data to be received
        if(count > 500)
        {
            return FALSE;
        }
    }
    if(UART_Rx_Buffer_PIR[0] != ACK)
    {
        return FAIL;
    }
    return PASS;
}

/******************************************************************************/
/* PIR_ReaPIR_ConfirmationCommanddCommand
 *
 * The function sends a confirmation command.
/******************************************************************************/
unsigned char PIR_ConfirmationCommand(const char Command)
{
    unsigned int count = 0;

    if(!PIR_ReadCommand(Command))
    {
        return FALSE;
    }
    if(UART_Rx_Buffer_PIR[0] != ACK)
    {
        return FAIL;
    }
    UART_PIR_CleanBuffer();
    RX_Response_PIR = FALSE;
    UART_PIR_SendStringConst(CONF_SEQUENCE);
    MSC_DelayUS(2000); // Wait for data to be received
    while(!RX_Response_PIR)
    {
        count++;
        MSC_DelayUS(20); // Wait for data to be received
        if(count > 500)
        {
            return FALSE;
        }
    }
    if(UART_Rx_Buffer_PIR[0] != ACK)
    {
        return FAIL;
    }
    return PASS;
}

/******************************************************************************/
/* PIR_Sleep
 *
 * The function puts the sensor to sleep of wakes the sensor.
/******************************************************************************/
void PIR_Sleep(unsigned char status)
{
    if(status)
    {
        /* turn on sleep mode */
        PIR_Asleep();
    }
    else
    {
        /* turn off sleep mode */
        PIR_Awake();
    }
}

/******************************************************************************/
/* PIR_Reset
 *
 * The function puts the PIR sensor into Serial Interface Mode.
/******************************************************************************/
void PIR_Reset(void)
{
    PIR_Sleep(TRUE);
    UART_PIR_TX_PIN(OFF);
    MSC_DelayUS(500);
    PIR_Sleep(FALSE);
    MSC_DelayUS(500);
    UART_PIR_TX_PIN(ON);
}

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/