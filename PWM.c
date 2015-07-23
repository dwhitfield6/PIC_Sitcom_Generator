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

#include "user.h"
#include "system.h"
#include "PWM.h"

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
 * Sets the duty cycly for the Red LED.
/******************************************************************************/
inline void PWM_SetRed(unsigned int Value)
{
    Red_Duty = Value;
}

/******************************************************************************/
/* PWM_SetGreen
 *
 * Sets the duty cycly for the Green LED.
/******************************************************************************/
inline void PWM_SetGreen(unsigned int Value)
{
    Green_Duty = Value;
}

/******************************************************************************/
/* PWM_SetBlue
 *
 * Sets the duty cycly for the Blue LED.
/******************************************************************************/
inline void PWM_SetBlue(unsigned int Value)
{
    Blue_Duty = Value;
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
 * The function initializes the 3 PWM channels.
/******************************************************************************/
void InitPWM(void)
{
#ifndef SitCom_Generator_PROTOBOARD
    
    RPOR10bits.RP21R    = 0b10100;    // RP19 = OC1 aka RED on RGB LED
    RPOR10bits.RP20R    = 0b10011;    // RP20 = OC1 aka GREEN on RGB LED
    RPOR9bits.RP19R     = 0b10010;    // RP21 = OC1 aka BLUE on RGB LED

    // Initialize Output Compare Module
    OC1CONbits.OCM = 0b000; // Disable Output Compare Module
    OC2CONbits.OCM = 0b000; // Disable Output Compare Module
    OC3CONbits.OCM = 0b000; // Disable Output Compare Module
    OC1R = 0; // Write the duty cycle for the first PWM pulse
    OC2R = 0; // Write the duty cycle for the first PWM pulse
    OC3R = 0; // Write the duty cycle for the first PWM pulse
    OC1RS = 200; // Write the duty cycle for the second PWM pulse
    OC2RS = 200; // Write the duty cycle for the second PWM pulse
    OC3RS = 200; // Write the duty cycle for the second PWM pulse
    OC1CONbits.OCTSEL = 0; // Select Timer 2 as output compare time base
    OC1CONbits.OCM = 0b110; // Select the Output Compare mode
    OC2CONbits.OCTSEL = 0; // Select Timer 2 as output compare time base
    OC2CONbits.OCM = 0b110; // Select the Output Compare mode
    OC3CONbits.OCTSEL = 0; // Select Timer 2 as output compare time base
    OC3CONbits.OCM = 0b110; // Select the Output Compare mode

    // Initialize and enable Timer2
    T2CONbits.TON = 0; // Disable Timer
    T2CONbits.TCS = 0; // Select internal instruction cycle clock
    T2CONbits.TGATE = 0; // Disable Gated Timer mode
    T2CONbits.TCKPS = 0b10; // Select 1:64 Prescaler
    TMR2 = 0x00; // Clear timer register
    PR2 = 1000; // Load the period value
    IPC1bits.T2IP = 0x01; // Set Timer 2 Interrupt Priority Level
    IFS0bits.T2IF = 0; // Clear Timer 2 Interrupt Flag
    IEC0bits.T2IE = 1; // Enable Timer 2 interrupt
    T2CONbits.TON = 1; // Start Timer
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
#ifndef SitCom_Generator_PROTOBOARD
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
}
#else
    Nop();
#endif
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/