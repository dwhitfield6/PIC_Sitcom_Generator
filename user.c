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
/* Contains Functions for PIC initialization
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
#include "MISC.h"
#include "ADC.h"
#include "Sound.h"


/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

/******************************************************************************/
/* Inline Functions
/******************************************************************************/

/******************************************************************************/
/* SD_SPI_GetCD
 *
 * The function gets the status of the Card Detector.
/******************************************************************************/
#ifdef CardDetect
inline bool SD_SPI_GetCD(void)
{
    if((PORTB & SD_CD) != SD_CD)
    {
        return ON;
    }
    return OFF;
}
#endif

/******************************************************************************/
/* SD_SPI_GetWP
 *
 * The function gets the status of the Card Detector.
/******************************************************************************/
#ifdef WriteProtect
inline bool SD_SPI_GetWP(void)
{
    if((PORTB & SD_WP) != SD_WP)
    {
        return ON;
    }
    return OFF;
}
#endif

/******************************************************************************/
/* SD_SPI_SetCS
 *
 * The function sets the Chip Select
/******************************************************************************/
inline void SD_SPI_SetCS(uint8_t a)
{
    if(a)
    {
        SD_CS_INACTIVE();
    }
    else
    {
        SD_CS_ACTIVE();
    }
}

/******************************************************************************/
/* Functions
/******************************************************************************/

/******************************************************************************/
/* InitApp
 *
 * The function initializes the application. It sets the pins and pull-ups.
/******************************************************************************/
void InitApp(void)
{
    LATA = 0;
    LATB = 0;
    
    RedLEDTris          = OUTPUT;
    AudioAmpMuteTris    = OUTPUT;
    AudioAmpStandbyTris = OUTPUT;

    ACLKCONbits.APSTSCLR = 5; // Auxiliary Clock Output Divider is 4
}

/******************************************************************************/
/* Init_System
 *
 * The function initializes the system
/******************************************************************************/
void Init_System (void)
{
    InitDAC();
}

/******************************************************************************/
/* SdSpiConfigurePins
 *
 * The function initializes the SPI pins used to communicate with the sd card.
/******************************************************************************/
void SdSpiConfigurePins (void)
{
    // Deassert the chip select pin
    SD_CS_INACTIVE();
    // Configure CS pin as an output
    SPI_SD_CSTris = OUTPUT;
#ifdef WriteProtect
    // Configure WP pin as an input
    SD_WPTris = INPUT;
#endif

#ifdef CardDetect
    // Configure CD pin as an input
    SD_CDTris = INPUT;
#endif
    RPINR20bits.SDI1R = 0x02; // SPI1 data input (MISO) is set to RP2
    RPOR3bits.RP7R = 0x07;    // RP7 = SDO aka MOSI
    RPOR4bits.RP8R = 0x08;    // RP8 = SDO aka SCK
}

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/