/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 05/29/15     1.0_DW0a    Initial project make.
/******************************************************************************/

/******************************************************************************/
/* Contains Functions for built-in Real-Time Clock and Calander module.
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
#include "MISC.h"
#include "RTCC.h"

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/
TIMETYPE CurrentTime;
TIMETYPE SetTime;

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

/******************************************************************************/
/* Inline Functions
/******************************************************************************/

/******************************************************************************/
/* Functions
/******************************************************************************/

/******************************************************************************/
/* Init_App
 *
 * The function initializes the Digital to analog convertor.
/******************************************************************************/
void InitRTCC(void)
{
    RTCC_ClearTime(&CurrentTime);
    RTCC_ClearTime(&SetTime);
   __builtin_write_OSCCONL(0x02); /* Enable Secondary Oscillator */
    RTCC_Access(TRUE);
    EnableRTCC();
    RTCC_Access(FALSE);
    RCFGCALbits.CAL = 0x00; /* No time Calibration */
}

/******************************************************************************/
/* RTCC_ClearTime
 *
 * The function clears the struct TIMETYPE.
/******************************************************************************/
void RTCC_ClearTime(TIMETYPE* Time)
{
    Time->Year = 0;
    Time->Month = 0;
    Time->Date = 0;
    Time->Weekday = 0;
    Time->Hour = 0;
    Time->Minute = 0;
    Time->Second = 0;
}

/******************************************************************************/
/* SetTime
 *
 * The function sets the time to the current time.
/******************************************************************************/
void RTCC_SetTime(void)
{
    SetTime.Year = 2015;
    SetTime.Month = May;
    SetTime.Date = 29;
    SetTime.Weekday = Friday;
    SetTime.Hour = 17;
    SetTime.Minute = 25;
    SetTime.Second = 0;
    RTCC_Write(SetTime);
}

/******************************************************************************/
/* RTCC_Access
 *
 * The function allows the user to update teh time and date by unlocking the
 *   RTC.
/******************************************************************************/
void RTCC_Access(unsigned char Yes_No)
{
    if(Yes_No == YES )
    {
        __builtin_write_RTCWEN(); /* Set RTCWREN bit */
    }
    else
    {
        asm("BCLR RCFGCAL,#13"); /* Clear RTCWREN bit */
    }
}

/******************************************************************************/
/* RTCC_Read
 *
 * The function reads the RTC.
/******************************************************************************/
void RTCC_Read(TIMETYPE* Time)
{
    unsigned int temp =0;

    RCFGCALbits.RTCPTR = 0x3; /* point to year */
    temp = RTCVAL;
    Time->Year = MSC_BCDtoHEX(temp & 0x00FF) + 2000;
    temp = RTCVAL;
    Time->Month = (unsigned char) MSC_BCDtoHEX((temp & 0xFF00) >> 8);
    Time->Date = (unsigned char) MSC_BCDtoHEX(temp & 0x00FF);
    temp = RTCVAL;
    Time->Weekday = (unsigned char) MSC_BCDtoHEX((temp & 0xFF00) >> 8);
    Time->Hour = (unsigned char) MSC_BCDtoHEX(temp & 0x00FF);
    temp = RTCVAL;
    Time->Minute = (unsigned char) MSC_BCDtoHEX((temp & 0xFF00) >> 8);
    Time->Second = (unsigned char) MSC_BCDtoHEX(temp & 0x00FF);
}

/******************************************************************************/
/* RTCC_Write
 *
 * The function allows the user to update teh time and date by unlocking the
 *   RTC.
/******************************************************************************/
void RTCC_Write(TIMETYPE Time)
{
    unsigned int temp =0;

    RTCC_Access(TRUE);

    RCFGCALbits.RTCPTR = 0x3; /* point to year */
    temp = MSC_HEXtoBCD( Time.Year - 2000);
    RTCVAL = temp;

    temp = (MSC_HEXtoBCD( (unsigned int) Time.Month) << 8) + (MSC_HEXtoBCD( (unsigned int) Time.Date));
    RTCVAL = temp;

    temp = (MSC_HEXtoBCD( (unsigned int) Time.Weekday) << 8) + (MSC_HEXtoBCD( (unsigned int) Time.Hour));
    RTCVAL = temp;

    temp = (MSC_HEXtoBCD( (unsigned int) Time.Minute) << 8) + (MSC_HEXtoBCD( (unsigned int) Time.Second));
    RTCVAL = temp;

    RTCC_Access(FALSE);
}

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/