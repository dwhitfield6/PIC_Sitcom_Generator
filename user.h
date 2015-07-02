/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 04/02/15     1.0_DW0a    Initial project make.
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#ifndef USER_H
#define	USER_H

#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

/******************************************************************************/
/* Info
 *
 * 1. Built-in functions here:
 * http://ww1.microchip.com/downloads/en/DeviceDoc/50002071E.pdf
 *
 * 
/******************************************************************************/

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
 * 1. SitCom_Generator_PROTOBOARD
/******************************************************************************/
#define SitCom_Generator_PROTOBOARD

/******************************************************************************/
/* WriteProtect
 *
 * Define this variable to tell the firmware that our sd card holder has a
 *   write protect detector switch.
/******************************************************************************/
//#define WriteProtect

/******************************************************************************/
/* CardDetect
 *
 * Define this variable to tell the firmware that our sd card holder has a
 *   Card detector switch.
/******************************************************************************/
//#define CardDetect

/******************************************************************************/
/* Version
 *
 * This defines the Firmware version.
/******************************************************************************/
#define Version "1.0_DW0a"

/******************************************************************************/
/* MAX_FILES
 *
 * This is the number of files that we are able to read.
/******************************************************************************/
#define MAX_FILES 20

/******************************************************************************/
/* SPI_FAST
 *
 * This is the SPI bus speed used during playback in kHz.
/******************************************************************************/
#define SPI_FAST 800.0

/******************************************************************************/
/* LOW_MEMORY
 *
 * This macro is used to leave out some of the functions when RAM is running
 * low.
/******************************************************************************/
#define LOW_MEMORY

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

/* Connected to the secondary oscillator  */
#define SOSCOTris	TRISBbits.TRISB4

/* Connected to the secondary oscillator  */
#define SOSCITris	TRISAbits.TRISA4

/* Connected to the Red Led  */
#define RedLEDTris	TRISBbits.TRISB10
#define RedLED 0x0400//RB10

/* Connected to Audio Amp mute */
#define AudioAmpMuteTris	TRISBbits.TRISB5
#define AudioAmpMute 0x0020//RB5

/* Connected to Audio Amp Standby */
#define AudioAmpStandbyTris	TRISBbits.TRISB6
#define AudioAmpStandby 0x0040//RB6

/* Connected to SPI MISO used for the SD card */
#define SPI_SD_MISOTris	TRISBbits.TRISB2
#define SPI_SD_MISO 0x0004//RB2 used as RP2

/* Connected to SPI MISO used for the SD card */
#define SPI_SD_MISO2Tris	TRISBbits.TRISB3
#define SPI_SD_MISO2 0x0008//RB3

/* Connected to SPI MOSI used for the SD card */
#define SPI_SD_MOSITris	TRISBbits.TRISB7
#define SPI_SD_MOSI 0x0080//RB14 used as RP7

/* Connected to SPI SCK used for the SD card */
#define SPI_SD_SCKTris	TRISBbits.TRISB8
#define SPI_SD_SCK 0x0100//RB15 used as RP8

/* Connected to SPI CS used for the SD card */
#define SPI_SD_CSTris	TRISBbits.TRISB9
#define SPI_SD_CS 0x0200//RB9 used as RP9

#ifdef WriteProtect
/* Connected to SD card holder Write Protect switch */
#define SD_WPTris	TRISBbits.TRISB9
#define SD_WP 0x0200//RB9 used as RP9
#endif

#ifdef CardDetect
/* Connected to SD card holder Write Protect switch */
#define SD_CDTris	TRISBbits.TRISB9
#define SD_CD 0x0200//RB9 used as RP9
#endif

/* DAC1RP and DAC1RN are used to play the sound and connect to the op-amp */

/******************************************************************************/
/* Macro Functions                                                            */
/******************************************************************************/

/******************************************************************************/
/* RedLEDON
 *
 * The function turns on the Red LED.
/******************************************************************************/
#define RedLEDON()  (LATB |= RedLED)

/******************************************************************************/
/* RedLEDOFF
 *
 * The function turns off the Red LED.
/******************************************************************************/
#define RedLEDOFF()  (LATB &= ~RedLED)

/******************************************************************************/
/* RedLEDTOGGLE
 *
 * The function toggles the Red LED.
/******************************************************************************/
#define RedLEDTOGGLE()  (LATB ^= RedLED)

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/
void Init_App(void);        
void Init_System (void);

#endif	/* USER_H */