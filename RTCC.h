/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 05/29/15     1.0_DW0a    Initial coding.
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#ifndef RTCC_H
#define	RTCC_H

#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

/******************************************************************************/
/* Structures                                                                 */
/******************************************************************************/
typedef struct time
{
    unsigned char Second;
    unsigned char Minute;
    unsigned char Hour;
    unsigned char Weekday;
    unsigned char Date;
    unsigned char Month;
    unsigned int Year;
}TIMETYPE;

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Time Macros                                                                */
/******************************************************************************/
#define January 1
#define February 2
#define March 3
#define April 4
#define May 5
#define June 6
#define July 7
#define August 8
#define September 9
#define October 10
#define November 11
#define December 12

#define Sunday 0
#define Monday 1
#define Tuesday 2
#define Wednesday 3
#define Thursday 4
#define Friday 5
#define Saturday 6

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
extern TIMETYPE CurrentTime;
extern TIMETYPE SetTime;

/******************************************************************************/
/* Macro Functions                                                            */
/******************************************************************************/

/******************************************************************************/
/* EnableRTCC
 *
 * The function enables the built-in Real time clock and calander.
/******************************************************************************/
#define EnableRTCC()  (RCFGCALbits.RTCEN = 1)

/******************************************************************************/
/* DisableRTCC
 *
 * The function disables the built-in Real time clock and calander.
/******************************************************************************/
#define DisableRTCC()  (RCFGCALbits.RTCEN = 0)

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/
void InitRTCC(void);
void RTCC_ClearTime(TIMETYPE* Time);
void RTCC_Access(unsigned char Yes_No);
void RTCC_Read(TIMETYPE* Time);
void RTCC_Write(TIMETYPE Time);
void RTCC_SetTime(void);

#endif	/* RTCC_H */