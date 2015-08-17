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
/* DAC_BUFFER_SIZE
 *
 * This is the maximum playback sample rate. Files with a sample rate higher
 * than this will be rejected.
/******************************************************************************/
#define WAV_SAMPLERATE_LIMIT 21000

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/
#define WAV_FINISHED 15
#define WAV_CONTINUE 16
#define WAV_FAIL 17

/******************************************************************************/
/* Macro Functions                                                            */
/******************************************************************************/

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
extern unsigned char ValidWAVFiles[MAX_FILES];
extern unsigned char WAV_DONE;
extern unsigned char WaveFilesNumHigh;
extern unsigned char WaveFilesNumLow;
extern unsigned char Valid_Wav;

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/
unsigned char WAV_ParseHeader(unsigned char* buffer, unsigned char fileNumber);
unsigned char WAV_CheckFiles(void);
unsigned char WAV_PlayFile_Random_Sector(unsigned char file);
unsigned char WAV_PlayFile_Continuous_Sector(unsigned char file);
unsigned char WAV_Continuous_Cluster(unsigned char file, unsigned char *first, unsigned char *status );
unsigned char WAV_MultipleBlockRead(long StartIndex, long StopIndex, unsigned char File, unsigned char *First, unsigned char *Status);
void WAV_TestAllFiles(void);

#endif	/* WAV_H */