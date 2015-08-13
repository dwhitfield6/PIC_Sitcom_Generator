/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 04/02/15     1.0_DW0a    Initial project make.
 *                          Derived from project 'PIC_Smart_Rf'.
 *                          Fixed bugs.
 *                          Added features.
/******************************************************************************/

/******************************************************************************/
/* Contains ISR
 *
 * Look here for the default vector names
 * c:\Program Files (x86)\Microchip\xc16\v1.24\support\dsPIC33F\gld\
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include <xc.h>        /* XC8 General Include File */

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#include "MISC.h"
#include "DAC.h"
#include "USER.h"
#include "START_SOUND.h"
#include "SPI.h"
#include "WAV.h"
#include "UART.h"
#include "PIR.h"
#include "PWM.h"
#include "SWITCH.h"

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/
#define _ISR_PSV __attribute__((interrupt, __auto_psv))
#define _ISR_NOPSV __attribute__((interrupt, no_auto_psv))

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/

/******************************************************************************/
/* DAC interrupt
/******************************************************************************/
void _ISR_NOPSV _DAC1RInterrupt(void)
{
    int temp;
    IFS4bits.DAC1RIF = 0; /* Clear Right Channel Interrupt Flag */
    MSC_TestPoint1TOGGLE();
    if(StartupSong)
    {
        temp = Start_Clip[DAC_Buffer_Place];
        if(temp != 0)
        {
            DAC_Buffer_Place++;
            temp -= 127;
            temp <<= 7; //Turn from unsigned to signed
            DAC1RDAT = temp;
        }
        else
        {
            DAC_Buffer_Place = 0;
            StartupSong = FALSE;
            ClipDone = TRUE;
            DAC1RDAT = 0x8000;
        }
    }
    else
    {
        if(DAC_Buffer_Place >= DAC_Buffer_Elements[DAC_Page_Read])
        {
            if(WAV_DONE)
            {
                ClipDone = TRUE;
            }
            DAC_Buffer_Place = 0;
            DAC_Page_Write_Finished[DAC_Page_Read] = FALSE; /* Read the page so it can now be written to */
            if(DAC_Page_Read == FIRST)
            {
                DAC_Page_Read = SECOND;
            }
            else
            {
                DAC_Page_Read = FIRST;
            }
        }
        if(DAC_Page_Write_Finished[DAC_Page_Read] == FALSE)
        {
            /* the read page caught the write page */
            /* this is an error if the sound file isn't finished playing */
            DAC_ERROR = TIMING;
        }
        temp = DAC_FIFO[DAC_Page_Read][DAC_Buffer_Place];
        temp >>= 2;
        DAC1RDAT = temp;
        DAC_Buffer_Place++;
    }

    if(ClipDone == TRUE)
    {
        DAC_OFF();
    }
}

/******************************************************************************/
/* SPI interrupt
/******************************************************************************/
void _ISR_NOPSV _SPI2Interrupt(void)
{
    IFS2bits.SPI2IF = 0;    // Clear the Interrupt flag
    SPI_State = FINISHED;
}

/******************************************************************************/
/* UART Receive interrupt from communication from PIR
/******************************************************************************/
void _ISR_NOPSV _U1RXInterrupt(void)
{
    unsigned char temp;

    RX_Response_PIR = TRUE;
    if(!U1STAbits.FERR)
    {
        /* there was no error */
        if(UART_Rx_Buffer_Place_PIR < UART_BUFFER_SIZE_PIR)
        {
            UART_Rx_Buffer_PIR[UART_Rx_Buffer_Place_PIR] = U1RXREG;
            UART_Rx_Buffer_Place_PIR++;
        }
        else
        {
            UART_PIR_CleanBuffer();
        }
    }
    else
    {
        U1STAbits.FERR = 0;
        temp = U1RXREG;
    }
    IFS4bits.U1EIF = 0; // clear error flag
    IFS0bits.U1RXIF = 0; // clear receive flag
}

/******************************************************************************/
/* UART Receive interrupt from communication from Debug port
/******************************************************************************/
void _ISR_NOPSV _U2RXInterrupt(void)
{
    unsigned char temp;

    if(!U2STAbits.FERR)
    {
        /* there was no error */
        if(UART_Rx_Buffer_Place_DEBUG < UART_BUFFER_SIZE_DEBUG)
        {
            UART_Rx_Buffer_DEBUG[UART_Rx_Buffer_Place_DEBUG] = U2RXREG;
            UART_Rx_Buffer_Place_DEBUG++;
        }
        else
        {
            UART_DEBUG_CleanBuffer();
        }
    }
    else
    {
        temp = U2RXREG;
    }
    IFS4bits.U2EIF = 0; // clear error flag
    IFS1bits.U2RXIF = 0; // clear receive flag
}

/******************************************************************************/
/* PIR Motion Detect interrupt
/******************************************************************************/
#ifndef PROTOBOARD
void _ISR_NOPSV _INT1Interrupt(void)
{
    Motion = TRUE;
    IFS1bits.INT1IF = 0; // clear flag
}
#else
void _ISR_NOPSV _CNInterrupt(void)
{
    Motion = TRUE;
    MSC_RedLEDON();
    IFS1bits.CNIF = 0; // clear flag
}
#endif

/******************************************************************************/
/* Door open Detect through optional switch interrupt
/******************************************************************************/
void _ISR_NOPSV _INT2Interrupt(void)
{

    DoorOpened = TRUE;
    IFS1bits.INT2IF = 0; // clear flag
}

/******************************************************************************/
/* PWM RED interrupt
/******************************************************************************/
void _ISR_NOPSV _T2Interrupt( void )
{
    /* Interrupt Service Routine code goes here */
    OC1RS = Red_Duty; // Write Duty Cycle value for next PWM cycle
    OC2RS = Green_Duty; // Write Duty Cycle value for next PWM cycle
    OC3RS = Blue_Duty; // Write Duty Cycle value for next PWM cycle
    IFS0bits.T2IF = 0; // Clear Timer 2 interrupt flag
}

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/