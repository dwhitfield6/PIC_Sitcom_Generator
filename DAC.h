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
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#ifndef DAC_H
#define	DAC_H

#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/
#define _8BIT 32
#define _16BIT 67

/******************************************************************************/
/* Union                                                                      */
/******************************************************************************/
typedef union
{
    unsigned int* uINT;
    unsigned char* uCHAR;
    int* INT;
    char* CHAR;
    const unsigned int* cuINT;
    const unsigned char* cuCHAR;
    const int* cINT;
    const char* cCHAR;
} Unkpointer;

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
extern volatile unsigned char ClipDone;
extern volatile unsigned char StartupSong;
extern unsigned int DAC_FIFO[2][256];
extern volatile unsigned char DAC_Page_Write;
extern volatile unsigned char DAC_Page_Read;
extern unsigned int DAC_Buffer_Place;
extern unsigned int DAC_Buffer_Elements[2];
extern volatile unsigned char DAC_Page_Write_Finished[2];

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/
#define FIRST      0
#define SECOND     1

/******************************************************************************/
/* Macro Functions                                                            */
/******************************************************************************/

/******************************************************************************/
/* DAC_ON
 *
 * The function turns on DAC module.
/******************************************************************************/
#define DAC_ON() (DAC1CONbits.DACEN = 1) /* DAC1 Module Enabled */

/******************************************************************************/
/* DAC_OFF
 *
 * The function turns off DAC module.
/******************************************************************************/
#define DAC_OFF() (DAC1CONbits.DACEN = 0) /* DAC1 Module disabled */

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/
void InitDAC(void);
void DAC_TurnOnAmp(void);
void DAC_TurnOffAmp(void);
void DAC_MuteAmp(void);
void DAC_UnMuteAmp(void);
void DAC_Voltage(unsigned int counts);
inline void DAC_Stop(void);
inline void DAC_Run(void);
void DAC_AudioOn(void);
void DAC_AudioOff(void);
void DAC_Play_Startup(void);
inline void DAC_ToggleWriteDACPage(void);

#endif	/* DAC_H */