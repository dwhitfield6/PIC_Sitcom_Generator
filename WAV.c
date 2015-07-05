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
#include "DAC.h"

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
unsigned char ValidWAVFiles[MAX_FILES];
unsigned char WAV_DONE = FALSE;

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
    tempL = MSC_EndianLongArray(&buffer[4]);
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
    tempL = MSC_EndianLongArray(&buffer[16]);
    FileList[fileNumber].WAV_DATA.Subchunk1Size = tempL;

    /* AudioFormat */
    tempI = MSC_EndianIntArray(&buffer[20]);
    FileList[fileNumber].WAV_DATA.AudioFormat = tempI;
    if(tempI != 0x01)
    {
        /* compressed */
        FileList[fileNumber].WAV_DATA.valid = FAIL;
        return FAIL;
    }

    /* NumChannels */
    tempI = MSC_EndianIntArray(&buffer[22]);
    FileList[fileNumber].WAV_DATA.NumChannels = tempI;

    /* SampleRate */
    tempL = MSC_EndianLongArray(&buffer[24]);
    FileList[fileNumber].WAV_DATA.SampleRate = tempL;

    /* ByteRate */
    tempL = MSC_EndianLongArray(&buffer[28]);
    FileList[fileNumber].WAV_DATA.ByteRate = tempL;

    /* Blockalign */

    /* BitsPerSample */
    tempI = MSC_EndianIntArray(&buffer[34]);
    FileList[fileNumber].WAV_DATA.BitsPerSample = tempI;

    /* Subchunk2ID */
    if(buffer[36] != 'd' || buffer[37] != 'a' || buffer[38] != 't' || buffer[39] != 'a')
    {
        FileList[fileNumber].WAV_DATA.valid = FAIL;
        return FAIL;
    }

    /* Subchunk2Size */
    tempL = MSC_EndianLongArray(&buffer[40]);
    FileList[fileNumber].WAV_DATA.Subchunk2Size = tempL;
    FileList[fileNumber].WAV_DATA.valid = PASS;
    FileList[fileNumber].WAV_DATA.NumSamples = FileList[fileNumber].WAV_DATA.Subchunk2Size/(FileList[fileNumber].WAV_DATA.NumChannels * (FileList[fileNumber].WAV_DATA.BitsPerSample >> 3));
    if(FileList[fileNumber].WAV_DATA.NumChannels == 2)
    {
        /* Stereo */
        if(FileList[fileNumber].WAV_DATA.BitsPerSample == 8)
        {
            /* 8 bit */
            FileList[fileNumber].WAV_DATA.SampleRepeat = 2;
        }
        else
        {
            /* 16 bit */
            FileList[fileNumber].WAV_DATA.SampleRepeat = 4;
        }
    }
    else
    {
        /* Mono */
        if(FileList[fileNumber].WAV_DATA.BitsPerSample == 8)
        {
            /* 8 bit */
            FileList[fileNumber].WAV_DATA.SampleRepeat = 1;
        }
        else
        {
            /* 16 bit */
            FileList[fileNumber].WAV_DATA.SampleRepeat = 2;
        }
    }
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
        if(WAV_ParseHeader(SD_Receive_Buffer_Big,i))
        {
            ValidWAVFiles[i] = PASS;
            found = TRUE;
        }
        else
        {
            ValidWAVFiles[i] = FAIL;
        }
    }
    return found;
}

/******************************************************************************/
/* WAV_PlayFile
 *
 * This plays the numbered audio file. If the ile is not valid then it returns
 * FAIL.
 *
 * 8-bit samples are stored as unsigned bytes, ranging from 0 to 255.
 * 16-bit samples are stored as 2's-complement signed integers, ranging from
 *   -32768 to 32767.
/******************************************************************************/
unsigned char WAV_PlayFile(unsigned char file)
{
    unsigned long cluster, fileSize, firstSector;
    unsigned long SamplesRead = 0;
    unsigned int DAC_Buffer_Count,SD_Buffer_Count;
    unsigned char j;
    unsigned char playing = FALSE;
    int temp;

    /* check to see if the file is out of range */
    if(file >= MAX_FILES)
    {
        return FAIL;
    }

    /* Check to see if the file is marked as valid */
    if(ValidWAVFiles[file] != PASS)
    {
        return FAIL;
    }

    cluster = FileList[file].firstCluster;
    fileSize = FileList[file].size;

    /* Read the first sector. This sector contains the header and some sound data */
    firstSector = FAT_GetFirstSector (cluster);
    if(!FAT_ReadSector(firstSector))
    {
        goto FINISHED;
    }

    /* data starts on byte 44 */
    DAC_Page_Write_Finished[FIRST] = FALSE;
    DAC_Page_Write_Finished[SECOND] = FALSE;
    DAC_Page_Write = FIRST;
    DAC_Page_Read = FIRST;
    DAC_Buffer_Place = 0;
    DAC_Buffer_Count = 0;
    SD_Buffer_Count = 44;
    WAV_DONE = FALSE;
    while(1)
    {
        if(FileList[file].WAV_DATA.BitsPerSample == 8)
        {
            temp = (SD_Receive_Buffer_Big[SD_Buffer_Count] - 127);
            temp <<= 8;
            DAC_FIFO[DAC_Page_Write][DAC_Buffer_Count] = temp;
        }
        else
        {
            DAC_FIFO[DAC_Page_Write][DAC_Buffer_Count] = MSC_EndianIntArray(&SD_Receive_Buffer_Big[SD_Buffer_Count]);
        }
        SD_Buffer_Count+=FileList[file].WAV_DATA.SampleRepeat;
        DAC_Buffer_Count++;
        SamplesRead++;
        if(SamplesRead >= FileList[file].WAV_DATA.NumSamples)
        {
            goto FINISHED;
        }
        if(SD_Buffer_Count >= FAT_BS.bytesPerSector)
        {
            break;
        }
        if(DAC_Buffer_Count >= DAC_BUFFER_SIZE)
        {
            DAC_Buffer_Elements[DAC_Page_Write] = DAC_Buffer_Count;
            DAC_Page_Write_Finished[DAC_Page_Write] = TRUE;
            DAC_Buffer_Count = 0;
            DAC_ToggleWriteDACPage();
            if(playing == FALSE)
            {
                /* Turn on the audio amp and start playback */
                DAC_SetClock((double)FileList[file].WAV_DATA.SampleRate);
                DAC_AudioOn();
                StartupSong = FALSE;
                ClipDone = FALSE;
                DAC_ON();
                playing = TRUE;
            }
        }
    }
    /* first sector read and saved to FIFO */

    /* read next sectors and fill up the FIFO */
    for (j=1; j<FAT_BS.sectorPerCluster; j++)
    {
        SD_Buffer_Count = 0;
        FAT_ReadSector(firstSector + j);
        while(1)
        {
            if(FileList[file].WAV_DATA.BitsPerSample == 8)
            {
                temp = (SD_Receive_Buffer_Big[SD_Buffer_Count] - 127);
                temp <<= 8;
                DAC_FIFO[DAC_Page_Write][DAC_Buffer_Count] = temp;
            }
            else
            {
                DAC_FIFO[DAC_Page_Write][DAC_Buffer_Count] = MSC_EndianIntArray(&SD_Receive_Buffer_Big[SD_Buffer_Count]);
            }
            SD_Buffer_Count+=FileList[file].WAV_DATA.SampleRepeat;
            DAC_Buffer_Count++;
            SamplesRead ++;
            if(SamplesRead >= FileList[file].WAV_DATA.NumSamples)
            {
                goto FINISHED;
            }
            if(SD_Buffer_Count >= FAT_BS.bytesPerSector)
            {
                break;
            }
            if(DAC_Buffer_Count >= DAC_BUFFER_SIZE)
            {
                DAC_Buffer_Elements[DAC_Page_Write] = DAC_Buffer_Count;
                DAC_Page_Write_Finished[DAC_Page_Write] = TRUE;
                DAC_Buffer_Count = 0;
                DAC_ToggleWriteDACPage();
                if(playing == FALSE)
                {
                    /* Turn on the audio amp and start playback */
                    DAC_SetClock((double)FileList[file].WAV_DATA.SampleRate);
                    DAC_AudioOn();
                    StartupSong = FALSE;
                    ClipDone = FALSE;
                    DAC_ON();
                    playing = TRUE;
                }
            }
        }
    }

    /* The first page of the sound file is loaded into the DAC FIFO */
    while(1)
    {
        /* get the next cluster */
        cluster = FAT_GetSetNextCluster (cluster, GET, NULL);
        firstSector = FAT_GetFirstSector (cluster);

        /* Read the sectors in the cluster */
        for (j=0; j<FAT_BS.sectorPerCluster; j++)
        {
            SD_Buffer_Count = 0;
            FAT_ReadSector(firstSector + j);
            while(1)
            {
                if(FileList[file].WAV_DATA.BitsPerSample == 8)
                {
                    temp = (SD_Receive_Buffer_Big[SD_Buffer_Count] - 127);
                    temp <<= 8;
                    DAC_FIFO[DAC_Page_Write][DAC_Buffer_Count] = temp;
                }
                else
                {
                    DAC_FIFO[DAC_Page_Write][DAC_Buffer_Count] = MSC_EndianIntArray(&SD_Receive_Buffer_Big[SD_Buffer_Count]);
                }
                SD_Buffer_Count+=FileList[file].WAV_DATA.SampleRepeat;
                DAC_Buffer_Count++;
                SamplesRead++;
                if(SamplesRead >= FileList[file].WAV_DATA.NumSamples)
                {
                    goto FINISHED;
                }
                if(SD_Buffer_Count >= FAT_BS.bytesPerSector)
                {
                    break;
                }
                if(DAC_Buffer_Count >= DAC_BUFFER_SIZE)
                {
                    DAC_Buffer_Elements[DAC_Page_Write] = DAC_Buffer_Count;
                    DAC_Page_Write_Finished[DAC_Page_Write] = TRUE;
                    DAC_Buffer_Count = 0;
                    DAC_ToggleWriteDACPage();
                    if(playing == FALSE)
                    {
                        /* Turn on the audio amp and start playback */
                        DAC_SetClock((double)FileList[file].WAV_DATA.SampleRate);
                        DAC_AudioOn();
                        StartupSong = FALSE;
                        ClipDone = FALSE;
                        DAC_ON();
                        playing = TRUE;
                    }
                }
            }
        }
    }

    FINISHED:
    WAV_DONE = TRUE;
    while(!ClipDone); /* wait till it stops playing */
    DAC_OFF();
    DAC_AudioOff();
    if(SamplesRead >= FileList[file].WAV_DATA.NumSamples)
    {
        return PASS;
    }
    return FAIL;
}


/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/