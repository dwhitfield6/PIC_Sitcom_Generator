/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 05/09/15     1.0_DW0a    Initial project make.
 *                          Ported over fileio example from microchip.
/******************************************************************************/

/******************************************************************************/
/* Contains main function.
 *
/******************************************************************************/


/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */
#include <stdio.h>       /* For true/false definition */
#include <string.h>

#include "system.h"
#include "user.h"
#include "MISC.h"
#include "ADC.h"
#include "Sound.h"
#include "rtcc.h"
#include "fileio.h"
#include "sd_spi.h"

/******************************************************************************/
/* Version number                                                             */
/******************************************************************************/

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
unsigned int AudioCounts = 0;
unsigned char Volume = 0;
unsigned int Audio = 0;

// The sdCardMediaParameters structure defines user-implemented functions needed by the SD-SPI fileio driver.
// The driver will call these when necessary.  For the SD-SPI driver, the user must provide
// parameters/functions to define which SPI module to use, Set/Clear the chip select pin,
// get the status of the card detect and write protect pins, and configure the CS, CD, and WP
// pins as inputs/outputs (as appropriate).
// For this demo, these functions are implemented in system.c, since the functionality will change
// depending on which demo board/microcontroller you're using.
// This structure must be maintained as long as the user wishes to access the specified drive.
FILEIO_SD_DRIVE_CONFIG sdCardMediaParameters =
{
    1,                                  // Use SPI module 1
    USER_SdSpiSetCs_1,                  // User-specified function to set/clear the Chip Select pin.
    USER_SdSpiGetCd_1,                  // User-specified function to get the status of the Card Detect pin.
    USER_SdSpiGetWp_1,                  // User-specified function to get the status of the Write Protect pin.
    USER_SdSpiConfigurePins_1           // User-specified function to configure the pins' TRIS bits.
};

// The gSDDrive structure allows the user to specify which set of driver functions should be used by the
// FILEIO library to interface to the drive.
// This structure must be maintained as long as the user wishes to access the specified drive.
const FILEIO_DRIVE_CONFIG gSdDrive =
{
    (FILEIO_DRIVER_IOInitialize)FILEIO_SD_IOInitialize,                      // Function to initialize the I/O pins used by the driver.
    (FILEIO_DRIVER_MediaDetect)FILEIO_SD_MediaDetect,                       // Function to detect that the media is inserted.
    (FILEIO_DRIVER_MediaInitialize)FILEIO_SD_MediaInitialize,               // Function to initialize the media.
    (FILEIO_DRIVER_MediaDeinitialize)FILEIO_SD_MediaDeinitialize,           // Function to de-initialize the media.
    (FILEIO_DRIVER_SectorRead)FILEIO_SD_SectorRead,                         // Function to read a sector from the media.
    (FILEIO_DRIVER_SectorWrite)FILEIO_SD_SectorWrite,                       // Function to write a sector to the media.
    (FILEIO_DRIVER_WriteProtectStateGet)FILEIO_SD_WriteProtectStateGet,     // Function to determine if the media is write-protected.
};

// Declare a state machine for our device
typedef enum
{
    DEMO_STATE_NO_MEDIA = 0,
    DEMO_STATE_MEDIA_DETECTED,
    DEMO_STATE_DRIVE_MOUNTED,
    DEMO_STATE_DONE,
    DEMO_STATE_FAILED
} DEMO_STATE;

// Some sample data to write to the file
uint8_t sampleData[10] = "DATA,10\r\n";

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

int main (void)
{
    DEMO_STATE demoState = DEMO_STATE_NO_MEDIA;
    FILEIO_OBJECT file;

    SYSTEM_Initialize();

    // Initialize the RTCC
    RTCCInit();

    // Initialize the library
    if (!FILEIO_Initialize())
    {
        while(1);
    }

    // Register the GetTimestamp function as the timestamp source for the library.
    FILEIO_RegisterTimestampGet (GetTimestamp);

    while(1)
    {
        switch (demoState)
        {
            case DEMO_STATE_NO_MEDIA:
                // Loop on this function until the SD Card is detected.
                if (FILEIO_MediaDetect(&gSdDrive, &sdCardMediaParameters) == true)
                {
                    demoState = DEMO_STATE_MEDIA_DETECTED;
                }
                break;
            case DEMO_STATE_MEDIA_DETECTED:
#if defined (__XC8__)
                // When initializing an SD Card on PIC18, the the SPI clock must be between 100 and 400 kHz.
                // The largest prescale divider in most PIC18 parts is 64x, which means the clock frequency must
                // be between 400 kHz (for a 4x divider) and 25.6 MHz (for a 64x divider).  In this demo, we are
                // achieving this by disabling the PIC's PLL during the drive mount operation (note that the
                // SYS_CLK_FrequencySystemGet() function must accurate reflect this change).  You could also map the "slow"
                // SPI functions used by the sd-spi driver to an SPI implementation that will run between
                // 100 kHz and 400 kHz at Fosc values greater than 25.6 MHz.  The macros to remap are located in
                // fileio_config.h.
                OSCTUNEbits.PLLEN = 0;
#endif
                // Try to mount the drive we've defined in the gSdDrive structure.
                // If mounted successfully, this drive will use the drive Id 'A'
                // Since this is the first drive we're mounting in this application, this
                // drive's root directory will also become the current working directory
                // for our library.
                if (FILEIO_DriveMount('A', &gSdDrive, &sdCardMediaParameters) == FILEIO_ERROR_NONE)
                {
                    demoState = DEMO_STATE_DRIVE_MOUNTED;
                }
                else
                {
                    demoState = DEMO_STATE_NO_MEDIA;
                }
#if defined (__XC8__)
                OSCTUNEbits.PLLEN = 1;
#endif
                break;
            case DEMO_STATE_DRIVE_MOUNTED:
                // Open TESTFILE.TXT.
                // Specifying CREATE will create the file is it doesn't exist.
                // Specifying APPEND will set the current read/write location to the end of the file.
                // Specifying WRITE will allow you to write to the code.
                if (FILEIO_Open (&file, "TESTFILE.CSV", FILEIO_OPEN_WRITE | FILEIO_OPEN_APPEND | FILEIO_OPEN_CREATE) == FILEIO_RESULT_FAILURE)
                {
                    demoState = DEMO_STATE_FAILED;
                    break;
                }

                // Write some sample data to the card
                if (FILEIO_Write (sampleData, 1, 9, &file) != 9)
                {
                    demoState = DEMO_STATE_FAILED;
                    break;
                }

                // Close the file to save the data
                if (FILEIO_Close (&file) != FILEIO_RESULT_SUCCESS)
                {
                    demoState = DEMO_STATE_FAILED;
                    break;
                }

                // We're done with this drive.
                // Unmount it.
                FILEIO_DriveUnmount ('A');
                demoState = DEMO_STATE_DONE;
                break;
            case DEMO_STATE_DONE:
                // Now that we've written all of the data we need to write in the application, wait for the user
                // to remove the card
                if (FILEIO_MediaDetect(&gSdDrive, &sdCardMediaParameters) == false)
                {
                    demoState = DEMO_STATE_NO_MEDIA;
                }
                break;
            case DEMO_STATE_FAILED:
                // An operation has failed.  Try to unmount the drive.  This will also try to
                // close all open files that use this drive (it will at least deallocate them).
                FILEIO_DriveUnmount ('A');
                // Return to the media-detect state
                demoState = DEMO_STATE_NO_MEDIA;
                break;
        }
    }
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/
