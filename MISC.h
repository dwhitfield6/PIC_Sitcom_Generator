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
void delayUS(long US);
unsigned char IsCharacter(unsigned char data);
void cleanBuffer(unsigned char* data, unsigned int count);
void cleanBuffer16bit(unsigned int* data, unsigned int count);
double Round(double input);
void BufferCopy(unsigned char* from,unsigned char* to, unsigned int count, unsigned int shift);
unsigned char StringMatch(const unsigned char* This, const unsigned char* That);
unsigned char StringMatchCaseInsensitive(const unsigned char* This, const unsigned char* That);
unsigned char StringContains(const unsigned char* This, const unsigned char* That);
unsigned char StringContainsCaseInsensitive(const unsigned char* This, const unsigned char* That);
void CalibrateDelay(void);
void lowercaseChar(unsigned char* Input);
void lowercaseString(unsigned char* Input);
void delayNOP(unsigned char NOPs);
unsigned char Reverse_Byte(unsigned char This);
unsigned int Reverse_2Byte(unsigned int This);
unsigned long Reverse_4Byte(unsigned long This);
unsigned char GetNumber(unsigned char* This, unsigned char CommaNumber, long* result);
unsigned char StringAddEqual(unsigned char* Input);
unsigned char GetStringAfterComma(unsigned char* This, unsigned char CommaNumber, unsigned char* result);
unsigned char GetNumberUnsigned(unsigned char* This, unsigned char CommaNumber, unsigned long* result);
unsigned char IsAlphaNumericString(unsigned char* data);
unsigned char IsAlphaNumeric(unsigned char data);
unsigned char IsNumber(unsigned char data);
double DBround(double Input);
long Scale(long Input, long InputRangeLow, long InputRangeHigh, long OutputRangeLow, long OutputRangeHigh);
unsigned int BCDtoHEX(unsigned int input);
unsigned int HEXtoBCD(unsigned int input);
unsigned char BufferMatch(unsigned char* This, unsigned char* That, unsigned int size);
long LGround(long Input,unsigned char places);
void TestFunctionPointer(void);

#endif	/* MISC_H */