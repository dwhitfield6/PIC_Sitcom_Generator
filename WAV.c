/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 06/26/15     1.0_DW0a    Initial coding.
 * /******************************************************************************/

/******************************************************************************/
/* Contains Functions for reading and parsing a WAV audio file.
 *
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
#include <stdio.h>         /* For sprintf definition */

#include "WAV.h"
#include "FAT.h"
#include "SD.h"
#include "MISC.h"
#include "user.h"

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
unsigned char ValidFiles[MAX_FILES];

/******************************************************************************/
/* Inline Functions
/******************************************************************************/

/******************************************************************************/
/* Functions
/******************************************************************************/

/******************************************************************************/
/* WAV_ParseHeader
 *
 * This function parses the header of the wav file and sets up the DAC to play
 * the audio.
/******************************************************************************/
unsigned char WAV_ParseHeader(unsigned char* buffer, unsigned char fileNumber)
{
    unsigned long tempL;
    unsigned int  tempI;
    /* ChinkID */
    if(buffer[0] != 'R' || buffer[1] != 'I' || buffer[2] != 'F' || buffer[3] != 'F')
    {
        FileList[fileNumber].WAV_DATA.valid = FAIL;
        return FAIL;
    }

    /* ChunkSize */
    tempL = Endian4ByteArray(&buffer[4]);
    FileList[fileNumber].WAV_DATA.ChunkSize = tempL;

    /* Format */
    if(buffer[8] != 'W' || buffer[9] != 'A' || buffer[10] != 'V' || buffer[11] != 'E')
    {
        FileList[fileNumber].WAV_DATA.valid = FAIL;
        return FAIL;
    }

    /* Subchunk1ID */
    if(buffer[12] != 'f' || buffer[13] != 'm' || buffer[14] != 't' || buffer[15] != ' ')
    {
        FileList[fileNumber].WAV_DATA.valid = FAIL;
        return FAIL;
    }
    
    /* Subchunk1Size */
    tempL = Endian4ByteArray(&buffer[16]);
    FileList[fileNumber].WAV_DATA.Subchunk1Size = tempL;

    /* AudioFormat */
    tempI = Endian2ByteArray(&buffer[20]);
    FileList[fileNumber].WAV_DATA.AudioFormat = tempI;
    if(tempI != 0x01)
    {
        /* compressed */
        FileList[fileNumber].WAV_DATA.valid = FAIL;
        return FAIL;
    }

    /* NumChannels */
    tempI = Endian2ByteArray(&buffer[22]);
    FileList[fileNumber].WAV_DATA.NumChannels = tempI;

    /* SampleRate */
    tempL = Endian4ByteArray(&buffer[24]);
    FileList[fileNumber].WAV_DATA.SampleRate = tempL;

    /* ByteRate */
    tempL = Endian4ByteArray(&buffer[28]);
    FileList[fileNumber].WAV_DATA.ByteRate = tempL;

    /* Blockalign */

    /* BitsPerSample */
    tempI = Endian2ByteArray(&buffer[32]);
    FileList[fileNumber].WAV_DATA.BitsPerSample = tempI;

    /* Subchunk2ID */
    if(buffer[36] != 'd' || buffer[37] != 'a' || buffer[38] != 't' || buffer[39] != 'a')
    {
        FileList[fileNumber].WAV_DATA.valid = FAIL;
        return FAIL;
    }

    /* Subchunk2Size */
    tempL = Endian4ByteArray(&buffer[40]);
    FileList[fileNumber].WAV_DATA.Subchunk2Size = tempL;
    FileList[fileNumber].WAV_DATA.valid = PASS;
    return PASS;
}

/******************************************************************************/
/* WAV_CheckFiles
 *
 * This function parses the header of the wav files and then sets flags in the
 * array saying if they are valid or not.
/******************************************************************************/
unsigned char WAV_CheckFiles(void)
{
    unsigned char i;
    unsigned long firstSector;
    unsigned char found = FALSE;

    for (i=0; i<MAX_FILES; i++)
    {
        firstSector = FAT_GetFirstSector (FileList[i].firstCluster);
        FAT_ReadSector(firstSector);
        if(WAV_ParseHeader(Receive_Buffer_Big,i))
        {
            ValidFiles[i] = PASS;
            found = TRUE;
        }
        else
        {
            ValidFiles[i] = FAIL;
        }
    }
    return found;
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/