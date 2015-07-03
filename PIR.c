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

#include "user.h"
#include "system.h"
#include "PIR.h"
#include "UART.h"
#include "MISC.h"

/******************************************************************************/
/* Confirmation Sequence                                                      */
/******************************************************************************/
const unsigned char CONF_SEQUENCE[] = { 1,2,3,4,0 };

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
volatile unsigned char Motion = FALSE;

/******************************************************************************/
/* Inline Functions                                                           */
/******************************************************************************/

/******************************************************************************/
/* PIR_Interrupt
 *
 * This function controls the change notification interrupt.
/******************************************************************************/
inline void PIR_Interrupt(unsigned char status)
{
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
    IPC4bits.CNIP = 2;  // Interrupt priority is 2
    PIR_Reset();        // Put the device into serial interface mode
    PIR_ReadCommand('a');
    MSC_DelayUS(2000); // Wait for data to be received
}

/******************************************************************************/
/* PIR_ReadCommand
 *
 * The function sends a read command.
/******************************************************************************/
unsigned char PIR_ReadCommand(const unsigned char Command)
{
    UART_CleanBuffer();
    RX_Response = FALSE;
    UART_SendCharConst(Command);
    MSC_DelayUS(2000); // Wait for data to be received
    if(!RX_Response)
    {
        return FALSE;
    }
    return TRUE;
}

/******************************************************************************/
/* PIR_WriteCommand
 *
 * The function sends a write command.
/******************************************************************************/
unsigned char PIR_WriteCommand(const unsigned char Command, unsigned char* CurrentValue, unsigned char* NewValue)
{
    if(!PIR_ReadCommand(Command))
    {
        return FALSE;
    }
    MSC_BufferCopy(UART_Receive_Buffer,CurrentValue, PIR_RESPONSE_SIZE, 0);
    UART_CleanBuffer();
    RX_Response = FALSE;
    UART_SendString(NewValue);
    MSC_DelayUS(2000); // Wait for data to be received
    if(!RX_Response)
    {
        return FAIL;
    }
    if(UART_Receive_Buffer[0] != ACK)
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
unsigned char PIR_ConfirmationCommand(const unsigned char Command)
{
    if(!PIR_ReadCommand(Command))
    {
        return FALSE;
    }
    if(UART_Receive_Buffer[0] != ACK)
    {
        return FAIL;
    }
    UART_CleanBuffer();
    RX_Response = FALSE;
    UART_SendStringConst(CONF_SEQUENCE);
    MSC_DelayUS(2000); // Wait for data to be received
    if(!RX_Response)
    {
        return FAIL;
    }
    if(UART_Receive_Buffer[0] != ACK)
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
    UART_TX_PIN(OFF);
    MSC_DelayUS(500);
    PIR_Sleep(FALSE);
    MSC_DelayUS(500);
    UART_TX_PIN(ON);
}

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/