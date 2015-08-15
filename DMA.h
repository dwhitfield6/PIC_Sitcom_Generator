/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 08/14/15     1.1_DW0a    Initial coding.
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#ifndef DMA_H
#define	DMA_H

#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
extern unsigned int DMA_RedDuty __attribute__((space(dma)));
extern unsigned int DMA_GreenDuty __attribute__((space(dma)));
extern unsigned int DMA_BlueDuty __attribute__((space(dma)));

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Macro Functions                                                            */
/******************************************************************************/

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/
void DMA_StartRGB(void);
void DMA_StartCH0(void);
void DMA_StartCH1(void);
void DMA_StartCH2(void);
void DMA_Channel0(void);
void DMA_Channel1(void);
void DMA_Channel2(void);

#endif	/* DMA_H */