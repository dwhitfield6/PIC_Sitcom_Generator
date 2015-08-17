/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 07/23/15     1.0_DW0a    Initial coding.
 * /******************************************************************************/

/******************************************************************************/
/* Contains Functions for the RGB LED control through PWM.
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
#include "PWM.h"
#include "TIMERS.h"
#include "DMA.h"

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
unsigned int Red_Duty = 300;
unsigned int Green_Duty = 300;
unsigned int Blue_Duty = 300;

/******************************************************************************/
/* Inline Functions                                                           */
/******************************************************************************/

/******************************************************************************/
/* PWM_SetRed
 *
 * Sets the duty cycle for the Red LED.
/******************************************************************************/
inline void PWM_SetRed(unsigned int Value)
{
    Red_Duty = Value;
    DMA_RedDuty = Red_Duty;
}

/******************************************************************************/
/* PWM_SetGreen
 *
 * Sets the duty cycle for the Green LED.
/******************************************************************************/
inline void PWM_SetGreen(unsigned int Value)
{
    Green_Duty = Value;
    DMA_GreenDuty = Green_Duty;
}

/******************************************************************************/
/* PWM_SetBlue
 *
 * Sets the duty cycle for the Blue LED.
/******************************************************************************/
inline void PWM_SetBlue(unsigned int Value)
{
    Blue_Duty = Value;
    DMA_BlueDuty = Blue_Duty;
}

/******************************************************************************/
/* PWM_SetRGB
 *
 * Sets the LED Color via 3 color maps.
/******************************************************************************/
inline void PWM_SetRGB(unsigned int Red, unsigned int Green, unsigned int Blue)
{
    PWM_SetRed(Red);
    PWM_SetGreen(Green);
    PWM_SetBlue(Blue);
}

/******************************************************************************/
/* Functions
/******************************************************************************/

/******************************************************************************/
/* InitPIR
 *
 * The function initializes the 3 PWM channels. This must be called after a
 * a call to initialize the timers.
/******************************************************************************/
void InitPWM(void)
{
#ifndef PROTOBOARD
    
    /* Red led is OC1 */
    /* Green led is OC2 */
    /* Blue led is OC3 */
    
    RPOR10bits.RP21R    = 0b10100;    // RP19 = OC1 aka RED on RGB LED
    RPOR10bits.RP20R    = 0b10011;    // RP20 = OC1 aka GREEN on RGB LED
    RPOR9bits.RP19R     = 0b10010;    // RP21 = OC1 aka BLUE on RGB LED

    // Initialize Output Compare Module
    OC1CONbits.OCM = 0b000; // Disable Output Compare Module
    OC2CONbits.OCM = 0b000; // Disable Output Compare Module
    OC3CONbits.OCM = 0b000; // Disable Output Compare Module
    OC1R = 0; // Write the duty cycle for the first PWM pulse of Red LED
    OC2R = 0; // Write the duty cycle for the first PWM pulse of Green LED
    OC3R = 0; // Write the duty cycle for the first PWM pulse of Blue LED
    OC1RS = 0; // Write the duty cycle for the second PWM pulse
    OC2RS = 0; // Write the duty cycle for the second PWM pulse
    OC3RS = 0; // Write the duty cycle for the second PWM pulse
    OC1CONbits.OCTSEL = 0; // Select Timer 2 as output compare time base
    OC1CONbits.OCM = 0b110; // Select the Output Compare mode
    OC2CONbits.OCTSEL = 0; // Select Timer 2 as output compare time base
    OC2CONbits.OCM = 0b110; // Select the Output Compare mode
    OC3CONbits.OCTSEL = 0; // Select Timer 2 as output compare time base
    OC3CONbits.OCM = 0b110; // Select the Output Compare mode

    #ifdef DMA_RGB
        DMA_StartRGB();    
    #else
        TMR_InterruptTimer2(ON);
    #endif
    
#else
    Nop();
#endif
}

/******************************************************************************/
/* PWM_SetColor
 *
 * Sets the LED Color by name.
/******************************************************************************/
void PWM_SetColor(unsigned int Color)
{
#ifndef PROTOBOARD
    switch (Color)
    {
        case RED:
            PWM_SetRGB(5,0,0);
            break;
        case GREEN:
            PWM_SetRGB(0,5,0);
            break;
        case BLUE:
            PWM_SetRGB(0,0,5);
            break;
        case PURPLE:
            PWM_SetRGB(15,0,2);
            break;
        case YELLOW:
            PWM_SetRGB(5,5,0);
            break;
         case WHITE:
            PWM_SetRGB(5,5,3);
            break;
        case TURQUOISE:
            PWM_SetRGB(0,5,3);
            break;
        default:
            PWM_SetRGB(0,0,0);
            break;
    }
#else
    Nop();
#endif
}


/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/