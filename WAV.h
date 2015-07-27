/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 06/26/15     1.0_DW0a    Initial coding.
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#ifndef WAV_H
#define	WAV_H

#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#include "WAV.h"
#include "FAT.h"
#include "SD.h"
#include "MISC.h"
#include "USER.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/
#define WAV_FINISHED 129
#define WAV_CONTINUE 110
#define WAV_FAIL 118

/******************************************************************************/
/* Macro Functions                                                            */
/******************************************************************************/

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
extern unsigned char ValidWAVFiles[MAX_FILES];
extern unsigned char WAV_DONE;
extern unsigned char WaveFilesNumHigh;

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/
unsigned char WAV_ParseHeader(unsigned char* buffer, unsigned char fileNumber);
unsigned char WAV_CheckFiles(void);
unsigned char WAV_PlayFile_Random_Sector(unsigned char file);
unsigned char WAV_PlayFile_Continuous_Sector(unsigned char file);
unsigned char WAV_Continuous_Cluster(unsigned char file, unsigned char *first, unsigned char *status );
unsigned char WAV_MultipleBlockRead(long StartIndex, long StopIndex, unsigned char File, unsigned char *First, unsigned char *Status);

#endif	/* WAV_H */