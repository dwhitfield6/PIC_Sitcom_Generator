/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 07/23/15     1.0_DW0a    Initial coding.
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#ifndef PWM_H
#define	PWM_H

#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

/******************************************************************************/
/* COLORS
 *
 * This is the available Colors to set the LED.
/******************************************************************************/
#define NONE        0
#define RED         1
#define YELLOW      2
#define GREEN       3
#define BLUE        4
#define PURPLE      5
#define WHITE       6
#define TURQUOISE   7

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
extern unsigned int Red_Duty;
extern unsigned int Green_Duty;
extern unsigned int Blue_Duty;

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Macro Functions                                                            */
/******************************************************************************/

/******************************************************************************/
/* PIR_Asleep
 *
 * The function puts the PIR sensor to sleep.
/******************************************************************************/
#define PIR_Asleep()  (LATA &= ~PIR_SLEEP)

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/
void InitPWM(void);
inline void PWM_SetRed(unsigned int Value);
inline void PWM_SetGreen(unsigned int Value);
inline void PWM_SetBlue(unsigned int Value);
inline void PWM_SetRGB(unsigned int Red, unsigned int Green, unsigned int Blue);
void PWM_SetColor(unsigned int Color);

#endif	/* PWM_H */