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
 * 04/09/15     1.0_DW0b    Fixed bugs.
 *                          Added features.
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#ifndef USER_H
#define	USER_H

#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/******************************************************************************/
/* Pick Which System!!!
 * 
 * Choose if the system has a bluetooth module or not.
 *
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

/******************************************************************************/
/* PCB board
 *
 * This code is used with boards:
 * 1. IR_to_RF_w_bluetooth_revA
/******************************************************************************/

/******************************************************************************/
/* Important parameters.
 *
 * NominalVDD is the voltage of the ldo powering the MCU. This is ued for the
 *  ADC reference voltage.
 *
 * VoltageLow is the low voltage threshold. 
 *
 * VoltageHigh is the high voltage threshold.
 *
/******************************************************************************/
#define NominalVDD 3.3
#define VoltageLow 6.3
#define VoltageHigh 16.5

/******************************************************************************/
/* BAUD
 *
 * This define the BAUD rate used by the system.
/******************************************************************************/
#define BAUD 115200

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

#define ON 1
#define OFF 0
#define TRUE 1
#define FALSE 0
#define INPUT 1
#define OUTPUT 0
#define PASS 1
#define FAIL 0
#define YES 1
#define NO 0

/******************************************************************************/
/* Pin Defines                                                                */
/******************************************************************************/
/* Connected to the Orange Led  */
#define OrangeLEDTris	TRISBbits.TRISB6
#define OrangeLED 0x40//RB6

/* Connected to Potentiometer  */
#define PotTris	TRISAbits.TRISA0
#define Pot 0x01//RA0 used as AD0
#define AN0 0x01//RA0 used as AD0

/* Connected to the 3.5mm stereo jack for audio input  */
#define AudioInputTris	TRISAbits.TRISA1
#define AudioInput 0x02//RA1 used as AD1
#define AN1 0x02//RA0 used as AD0

/* Connected to Audio Amp mute */
#define AudioAmpMuteTris	TRISBbits.TRISB5
#define AudioAmpMute 0x20//RB5

/* Connected to Audio Amp Standby */
#define AudioAmpStandbyTris	TRISAbits.TRISA4
#define AudioAmpStandby 0x10//RA4


/******************************************************************************/
/* Macro Functions                                                            */
/******************************************************************************/

/* OrangeLEDON()
 *
 * The function turns on the Orange LED.
/******************************************************************************/
#define OrangeLEDON()  (LATB |= OrangeLED)

/* OrangeLEDOFF()
 *
 * The function turns off the Orange LED.
/******************************************************************************/
#define OrangeLEDOFF()  (LATB &= ~OrangeLED)

/* OrangeLEDTOGGLE()
 *
 * The function toggles the Orange LED.
/******************************************************************************/
#define OrangeLEDTOGGLE()  (LATB ^= OrangeLED)

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/
void InitApp(void);        
void Init_System (void);

#endif	/* USER_H */