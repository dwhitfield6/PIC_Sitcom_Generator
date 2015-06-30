/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 04/02/15     1.0_DW0a    Initial project make.
 *                          Derived from project 'PIC_PS2_to_UART'.
 *                          Fixed bugs.
 *                          Added features.
 *                          Add new function to check for alphanumeric and
 *                            number.
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#ifndef MISC_H
#define	MISC_H

#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#include "system.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

#define SYS_FREQ_US 64
#define Even        2
#define Odd         3
#define NOCOMMA     1
#define DOUBLECOMMA 2
#define NOVALUE     3
#define TOOBIG      4
#define NEGATIVE    5
#define NoError     0


/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/
void MSC_DelayUS(long US);
unsigned char MSC_IsCharacter(unsigned char data);
void MSC_CleanBuffer(unsigned char* data, unsigned int count);
void MSC_CleanBufferInt(unsigned int* data, unsigned int count);
double MSC_Round(double input);
void MSC_BufferCopy(unsigned char* from,unsigned char* to, unsigned int count, unsigned int shift);
unsigned char MSC_StringMatch(const unsigned char* This, const unsigned char* That);
unsigned char MSC_StringMatchCaseInsensitive(const unsigned char* This, const unsigned char* That);
unsigned char MSC_StringContains(const unsigned char* This, const unsigned char* That);
unsigned char MSC_StringContainsCaseInsensitive(const unsigned char* This, const unsigned char* That);
void MSC_CalibrateDelay(void);
void MSC_LowercaseChar(unsigned char* Input);
void MSC_LowercaseString(unsigned char* Input);
void MSC_DelayNOP(unsigned char NOPs);
unsigned char MSC_ReverseByte(unsigned char This);
unsigned int MSC_ReverseInt(unsigned int This);
unsigned long MSC_ReverseLong(unsigned long This);
unsigned char MSC_GetNumber(unsigned char* This, unsigned char CommaNumber, long* result);
unsigned char MSC_StringAddEqual(unsigned char* Input);
unsigned char MSC_GetStringAfterComma(unsigned char* This, unsigned char CommaNumber, unsigned char* result);
unsigned char MSC_GetNumberUnsigned(unsigned char* This, unsigned char CommaNumber, unsigned long* result);
unsigned char MSC_IsAlphaNumericString(unsigned char* data);
unsigned char MSC_IsAlphaNumeric(unsigned char data);
unsigned char MSC_IsNumber(unsigned char data);
double MSC_DB_Round(double Input);
long MSC_Scale(long Input, long InputRangeLow, long InputRangeHigh, long OutputRangeLow, long OutputRangeHigh);
unsigned int MSC_BCDtoHEX(unsigned int input);
unsigned int MSC_HEXtoBCD(unsigned int input);
unsigned char MSC_BufferMatch(unsigned char* This, unsigned char* That, unsigned int size);
long MSC_LG_Round(long Input,unsigned char places);
void MSC_TestFunctionPointer(void);
unsigned int MSC_EndianInt(unsigned int number);
unsigned int MSC_EndianIntArray(unsigned char* buffer);
unsigned long MSC_EndianLong(unsigned long number);
unsigned long MSC_EndianLongArray(unsigned char* buffer);

#endif	/* MISC_H */