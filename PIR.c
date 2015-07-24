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
 * This function controls the external interrupt INT1 for pcb versions and the
 * change input interrupt for proto version.
/******************************************************************************/
inline void PIR_Interrupt(unsigned char status)
{
#ifndef SitCom_Generator_PROTOBOARD
    if(status)
    {
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
#ifndef SitCom_Generator_PROTOBOARD
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
#ifndef SitCom_Generator_PROTOBOARD
    /* the PIR module MD pin indicating motion is triggered by INT1 */
    INTCON2bits.INT1EP = 1; // trigger on negative edge
    IPC5bits.INT1IP = 2; // priority is 2
    RPINR0bits.INT1R = PIR_MD_RP;
#else
    IPC4bits.CNIP = 2;  // Interrupt priority is 2
#endif
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
    UART_PIR_CleanBuffer();
    RX_Response_PIR = FALSE;
    UART_PIR_SendCharConst(Command);
    MSC_DelayUS(2000); // Wait for data to be received
    if(!RX_Response_PIR)
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
    MSC_BufferCopy(UART_Rx_Buffer_PIR,CurrentValue, PIR_RESPONSE_SIZE, 0);
    UART_PIR_CleanBuffer();
    RX_Response_PIR = FALSE;
    UART_PIR_SendString(NewValue);
    MSC_DelayUS(2000); // Wait for data to be received
    if(!RX_Response_PIR)
    {
        return FAIL;
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
unsigned char PIR_ConfirmationCommand(const unsigned char Command)
{
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
    if(!RX_Response_PIR)
    {
        return FAIL;
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