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
 * 1. SitCom_Generator_PROTOBOARD (use configuration "Proto_pic33FJ128gp802")
 * 2. SitCom_Generator_PCB_REVA   (use configuration "PCB_revA_pic33FJ128gp804")
/******************************************************************************/
//#define SitCom_Generator_PROTOBOARD
#define SitCom_Generator_PCB_REVA

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
#define MAX_FILES 10

/******************************************************************************/
/* SPI_FAST
 *
 * This is the SPI bus speed used during playback in kHz.
/******************************************************************************/
#define SPI_FAST 400.0

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

/************* OScillator *************/
/* Connected to the secondary oscillator  */
#define SOSCOTris	TRISBbits.TRISB4

/* Connected to the secondary oscillator  */
#define SOSCITris	TRISAbits.TRISA4

/************* Red LED over GPIO *************/
/* Connected to the Red Led  */
#define RedLEDTris	TRISBbits.TRISB10
#define RedLED 0x0400 //RB10

#ifndef SitCom_Generator_PROTOBOARD
/************* RGB LED over PWM *************/
/* Connected to the RGB Red Led  */
#define RGB_RedTris	TRISCbits.TRISC5
#define RBG_Red 0x0008 //RC3 used as RP19

/* Connected to the RGB Green Led  */
#define RGB_GreenTris	TRISCbits.TRISC4
#define RBG_Green 0x0010 //RC4 used as RP20

/* Connected to the RGB Blue Led  */
#define RGB_BlueTris	TRISCbits.TRISC3
#define RBG_Blue 0x0020 //RC5 used as RP21
#endif

/************* DAC Right channel *************/
/* Connected to Audio Amp IN+ */
#define DAC_RightPTris	TRISBbits.TRISB14
#define DAC_RightP 0x1000 //RB12

#ifndef SitCom_Generator_PROTOBOARD
/* Connected to Audio Amp IN- */
#define DAC_RightNTris	TRISBbits.TRISB13
#define DAC_RightN 0x1000 //RB13
#endif

/************* Audio Amp over GPIO *************/
#ifdef SitCom_Generator_PROTOBOARD
/* Connected to Audio Amp mute on protoboard only */
#define AudioAmpMuteTris	TRISBbits.TRISB5
#define AudioAmpMute 0x0020 //RB5

/* Connected to Audio Amp Standby */
#define AudioAmpStandbyTris	TRISBbits.TRISB6
#define AudioAmpStandby 0x0040 //RB6
#else
/* Connected to Audio Amp Diagnostics on PCB only */
#define AudioAmpDiagTris	TRISBbits.TRISB5
#define AudioAmpDiag 0x0020 //RB5

/* Connected to Audio Amp Standby */
#define AudioAmpStandbyTris	TRISBbits.TRISB6
#define AudioAmpStandby 0x0040 //RB6
#endif

#ifndef SitCom_Generator_PROTOBOARD
/************* Test Point *************/
/* Connected to Test Point 1 on PCB only */
#define TP1_Tris	TRISAbits.TRISA8
#define TP1 0x0100 //RA8

/* Connected to Test Point 2 on PCB only */
#define TP2_Tris	TRISAbits.TRISA7
#define TP2 0x0080 //RA7

/* Connected to Test Point 3 on PCB only */
#define TP3_Tris	TRISAbits.TRISA10
#define TP3 0x0400 //RA10

/* Connected to Test Point 4 on PCB only */
#define TP4_Tris	TRISAbits.TRISA9
#define TP4 0x0200 //RA9
#endif

/************* Input Voltage over ADC *************/
#ifndef SitCom_Generator_PROTOBOARD
/* Connected from VIN to Voltage divider to ADC on PCB only */
#define ADC_VINTris	TRISCbits.TRISC2
#define ADC_VIN 0x0004 //RC2 used as AN8
#define ADC_VIN_AN 0x0100 //AN8

/* Connected from 5 volt rail to Voltage divider to ADC on PCB only */
#define ADC_5INTris	TRISBbits.TRISB14
#define ADC_5IN 0x4000 //RB14 used as AN10
#define ADC_5IN_AN 0x0400 //AN10
#endif

/************* SD Card over SPI *************/
#ifdef SitCom_Generator_PROTOBOARD
/* Connected to SPI MISO used for the SD card */
#define SPI_SD_MISOTris	TRISBbits.TRISB2
#define SPI_SD_MISO 0x0004 //RB2 used as RP2
#define SPI_SD_MISO_RP 2

/* Connected to SPI MISO used for the SD card */
#define SPI_SD_MISO2Tris	TRISBbits.TRISB3
#define SPI_SD_MISO2 0x0008 //RB3

/* Connected to SPI MOSI used for the SD card */
#define SPI_SD_MOSITris	TRISBbits.TRISB7
#define SPI_SD_MOSI 0x0080 //RB7 used as RP7

/* Connected to SPI SCK used for the SD card */
#define SPI_SD_SCKTris	TRISBbits.TRISB8
#define SPI_SD_SCK 0x0100 //RB8 used as RP8

/* Connected to SPI CS used for the SD card */
#define SPI_SD_CSTris	TRISBbits.TRISB9
#define SPI_SD_CS 0x0200 //RB9 used as RP9
#else
/* Connected to SPI MISO used for the SD card */
#define SPI_SD_MISOTris	TRISBbits.TRISB5
#define SPI_SD_MISO 0x0020 //RB5 used as RP5
#define SPI_SD_MISO_RP 5

/* Connected to SPI MISO used for the SD card */
#define SPI_SD_MISO2Tris	TRISBbits.TRISB6
#define SPI_SD_MISO2 0x0040 //RB6

/* Connected to SPI MOSI used for the SD card */
#define SPI_SD_MOSITris	TRISBbits.TRISB8
#define SPI_SD_MOSI 0x0100 //RB8 used as RP8

/* Connected to SPI SCK used for the SD card */
#define SPI_SD_SCKTris	TRISBbits.TRISB7
#define SPI_SD_SCK 0x0080 //RB7 used as RP7

/* Connected to SPI CS used for the SD card */
#define SPI_SD_CSTris	TRISBbits.TRISB9
#define SPI_SD_CS 0x0200 //RB9 used as RP9

/* Connected to SD Card Power on PCB only */
#define SD_PowerTris	TRISCbits.TRISC0
#define SD_Power 0x0001 //RC0

/* Connected to SD Connector switch on PCB only */
#define SD_INSERT_Tris	TRISCbits.TRISC6
#define SD_INSERT 0x0040 //RC6 used as CN18
#endif

/************* PIR sensor over UART *************/
#ifdef SitCom_Generator_PROTOBOARD
/* Connected to UART RX on PIR sensor */
#define PIR_TX_Tris	TRISBbits.TRISB15
#define PIR_TX 0x8000 //RB15 used as RP15


/* Connected to UART TX on PIR sensor */
#define PIR_RX_Tris	TRISBbits.TRISB11
#define PIR_RX 0x0800 //RB11 used as RP11
#define PIR_RX_RP 11

/* Connected to Motion Detect on PIR sensor */
#define PIR_MD_Tris	TRISAbits.TRISA0
#define PIR_MD 0x0001 //RA0 used as CN2
#define PIR_MD_CN 2

/* Connected to Motion Detect on PIR sensor */
#define PIR_SLEEP_Tris	TRISAbits.TRISA1
#define PIR_SLEEP 0x0002 //RA1
#else
/* Connected to UART RX on PIR sensor */
#define PIR_TX_Tris	TRISBbits.TRISB11
#define PIR_TX 0x8000 //RB11 used as RP15


/* Connected to UART TX on PIR sensor */
#define PIR_RX_Tris	TRISBbits.TRISB11
#define PIR_RX 0x0800 //RB11 used as RP11
#define PIR_RX_RP 11

/* Connected to Motion Detect on PIR sensor */
#define PIR_MD_Tris	TRISCbits.TRISC1
#define PIR_MD 0x0002 //RC1 used as RP17
#define PIR_MD_RP 17

/* Connected to Motion Detect on PIR sensor */
#define PIR_SLEEP_Tris	TRISAbits.TRISA1
#define PIR_SLEEP 0x0002 //RA1

/* Connected to Motion Detect on PIR sensor on PCB only */
#define PIR_MD2_Tris	TRISAbits.TRISA0
#define PIR_MD2 0x0001 //RA0
#endif

#ifndef SitCom_Generator_PROTOBOARD
/************* Debug UART *************/
/* Connected to UART RX on Debug Port */
#define DBG_TX_Tris	TRISCbits.TRISC8
#define DBG_TX 0x0100 //RC8 used as RP24


/* Connected to UART TX on Debug Port */
#define DBG_RX_Tris	TRISCbits.TRISC7
#define DBG_RX 0x0080 //RC7 used as RP23
#define DBG_RX_RP 23
#endif

#ifndef SitCom_Generator_PROTOBOARD
/************* Optional Door Switch *************/
/* Connected to Optional Door Switch */
#define DOOR_SW_Tris	TRISCbits.TRISC9
#define DOOR_SW 0x0200 //RC9 used as CN19
#define DOOR_SW_RP 25
#endif

/******************************************************************************/
/* Macro Functions                                                            */
/******************************************************************************/

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/
void Init_App(void);        
void Init_System (void);

#endif	/* USER_H */