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
/* This is the SPI driver file for dsPIC.
 *
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include <xc.h>
#include "drv_spi.h"
#include "system_config.h"
#include "system.h"
#include <stdint.h>

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
static int spiMutex[4] = { 0, 0, 0, 0 };

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/
static inline __attribute__((__always_inline__)) void DRV_SPI_WaitForDataByte1 (void);

/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/

/******************************************************************************/
/* DRV_SPI_Initialize
 *
 * Initializes the spi driver.
/******************************************************************************/
void DRV_SPI_Initialize(DRV_SPI_INIT_DATA *pData)
{     
    DRV_SPI_STAT(1) = 0;

    DRV_SPI_CON(1) = 0;
    DRV_SPI_CONbits(1).MSTEN = SPI_MST_MODE_ENABLE;
    DRV_SPI_CONbits(1).MODE16 = (pData->mode)& 0x01;
    DRV_SPI_CONbits(1).CKE = pData->cke;
    switch (pData->spibus_mode)
    {
        case SPI_BUS_MODE_0:
            //smp = 0, ckp = 0
            DRV_SPI_CONbits(1).CKP = 0;
            DRV_SPI_CONbits(1).SMP = 0;
            break;
        case SPI_BUS_MODE_1:
            //smp = 1, ckp = 0
            DRV_SPI_CONbits(1).CKP = 0;
            DRV_SPI_CONbits(1).SMP = 1;
            break;
        case SPI_BUS_MODE_2:
            //smp = 0, ckp = 1
            DRV_SPI_CONbits(1).CKP = 1;
            DRV_SPI_CONbits(1).SMP = 0;
            break;
        case SPI_BUS_MODE_3:
            //smp = 1, ckp = 1
            DRV_SPI_CONbits(1).CKP = 1;
            DRV_SPI_CONbits(1).SMP = 1;
            break;
        default:
            // should not happen
            break;
    }
    DRV_SPI_CON2(1) = 0;
    DRV_SPI_CONbits(1).PPRE= pData->primaryPrescale;
    DRV_SPI_CONbits(1).SPRE= pData->secondaryPrescale;
    DRV_SPI_STATbits(1).SPIEN = SPI_MODULE_ENABLE;
    return;
}

/******************************************************************************/
/* DRV_SPI_Deinitialize
 *
 * Turn off the SPI module.
/******************************************************************************/
void DRV_SPI_Deinitialize (uint8_t channel)
{
    DRV_SPI_STATbits(1).SPIEN = 0;
}

/******************************************************************************/
/* DRV_SPI_WaitForDataByte1
 *
 * Waits for the Receive to complete.
/******************************************************************************/
static inline __attribute__((__always_inline__)) void DRV_SPI_WaitForDataByte1 (void)
{
    while (!DRV_SPI_STATbits(1).SPIRBF);
}

/******************************************************************************/
/* DRV_SPI_Put
 *
 * Transmits one byte of data out of the SPI bus.
/******************************************************************************/
void DRV_SPI_Put(uint8_t channel, uint8_t data)
{
    uint8_t clear;

    while(DRV_SPI_STATbits(1).SPITBF);
    DRV_SPI_BUF(1) = data;

    // Wait for data uint8_t
    DRV_SPI_WaitForDataByte1();
    clear = DRV_SPI_BUF(1);
    return;
}

/******************************************************************************/
/* DRV_SPI_PutBuffer
 *
 * Transmits a buffer of data out of the SPI bus.
/******************************************************************************/
void DRV_SPI_PutBuffer(uint8_t channel, uint8_t * data, uint16_t count)
{
    uint8_t dataByte;
    if (count == 0)
    {
        return;
    }

    while (count--)
    {
        DRV_SPI_BUF(1) = *data++;
        DRV_SPI_WaitForDataByte1();
        dataByte = DRV_SPI_BUF(1);
    }
    return;
}

/******************************************************************************/
/* DRV_SPI_Get
 *
 * Retrieves a byte of data.
/******************************************************************************/
uint8_t DRV_SPI_Get (uint8_t channel)
{
    DRV_SPI_BUF(1) = 0xFF;
    DRV_SPI_WaitForDataByte1();
    return DRV_SPI_BUF(1);
}

/******************************************************************************/
/* DRV_SPI_GetBuffer
 *
 * Retrieves a buffer of length count over the spi bus.
/******************************************************************************/
void DRV_SPI_GetBuffer(uint8_t channel, uint8_t * data, uint16_t count)
{
   
    if (count == 0)
    {
        return;
    }
    while (count--)
    {
        DRV_SPI_BUF(1) = 0xFF;
        DRV_SPI_WaitForDataByte1();
        *data++ = DRV_SPI_BUF(1);
    }
    return;
}

/******************************************************************************/
/* DRV_SPI_Lock
 *
 * Locks the SPI bus.
/******************************************************************************/
int DRV_SPI_Lock(uint8_t channel)
{
    if(!spiMutex[0])
    {
        spiMutex[0] = 1;
        return 1;
    }

    return 0;
}

/******************************************************************************/
/* DRV_SPI_Unlock
 *
 * Unlocks the SPI bus.
/******************************************************************************/
void DRV_SPI_Unlock(uint8_t channel)
{
    spiMutex[0] = 0;    
}

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/