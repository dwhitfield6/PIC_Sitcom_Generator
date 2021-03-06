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
#include "USER.h"
#include "DAC.h"
#include "SPI.h"

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
unsigned char ValidWAVFiles[MAX_FILES];
unsigned char WAV_DONE = FALSE;
unsigned char WaveFilesNumHigh = 0;
unsigned char WaveFilesNumLow = 0;
unsigned long SamplesRead = 0;
unsigned char playing = FALSE;
unsigned int DAC_Buffer_Count = 0;
unsigned int SD_Buffer_Count = 0;
unsigned char Valid_Wav = FALSE;

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

    WaveFilesNumHigh = 0;
    WaveFilesNumLow = 255;
    for (i=0; i<MAX_FILES; i++)
    {
        firstSector = FAT_GetFirstSector (FileList[i].firstCluster);
        FAT_ReadSector(firstSector);
        if(WAV_ParseHeader(SD_Receive_Buffer_Big,i))
        {
            /* Valid wave file found */
            if(FileList[i].WAV_DATA.SampleRate <=WAV_SAMPLERATE_LIMIT)
            {
                ValidWAVFiles[i] = PASS;
                found = TRUE;
                if(WaveFilesNumLow == 255)
                {
                    WaveFilesNumLow = i;
                }
                WaveFilesNumHigh = i;
            }
            else
            {
                /* Wav sample rate is too high to playback */
               ValidWAVFiles[i] = FAIL;
            }
        }
        else
        {
            ValidWAVFiles[i] = FAIL;
        }
    }
    if(found == FALSE)
    {
        WaveFilesNumHigh = 0;
        WaveFilesNumLow  = 0;
        Valid_Wav = FALSE;
    }
    else
    {
        Valid_Wav = TRUE;
    }
    return found;
}

/******************************************************************************/
/* WAV_PlayFile_Random_Sector
 *
 * This plays the numbered audio file. If the file is not valid then it returns
 * FAIL.
 *
 * The method used to read the SD card is by random sector. This means that the
 *   sectors are read one at a time. This is a slow method! A faster method
 *   would be to do a continuous read for all of the sectors in the cluster.
 *   This would save the overhead of telling the sd card each sector to read.
 *
 * 8-bit samples are stored as unsigned bytes, ranging from 0 to 255.
 * 16-bit samples are stored as 2's-complement signed integers, ranging from
 *   -32768 to 32767.
/******************************************************************************/
unsigned char WAV_PlayFile_Random_Sector(unsigned char file)
{
    unsigned long cluster, fileSize, firstSector;
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
        goto Finish;
    }

    /* data starts on byte 44 */
    DAC_ERROR =0; // clear the DAC error
    DAC_Page_Write_Finished[FIRST] = FALSE;
    DAC_Page_Write_Finished[SECOND] = FALSE;
    DAC_Page_Write = FIRST;
    DAC_Page_Read = FIRST;
    DAC_Buffer_Place = 0;
    DAC_Buffer_Count = 0;
    SD_Buffer_Count = 44;
    WAV_DONE = FALSE;
    playing = FALSE;
    SamplesRead = 0;
    while(1)
    {
        if(DAC_ERROR)
        {
           Nop();
           goto Finish;
        }
        if(FileList[file].WAV_DATA.BitsPerSample == 8)
        {
            temp = (SD_Receive_Buffer_Big[SD_Buffer_Count] - 127);
            temp <<= 8;
            DAC_FIFO[DAC_Page_Write][DAC_Buffer_Count] = temp;
        }
        else
        {
            DAC_FIFO[DAC_Page_Write][DAC_Buffer_Count] = (int) MSC_EndianIntArray(&SD_Receive_Buffer_Big[SD_Buffer_Count]);
        }
        SD_Buffer_Count+=FileList[file].WAV_DATA.SampleRepeat;
        DAC_Buffer_Count++;
        SamplesRead++;
        if(SamplesRead >= FileList[file].WAV_DATA.NumSamples)
        {
            DAC_Buffer_Elements[DAC_Page_Write] = DAC_Buffer_Count;
            DAC_Page_Write_Finished[DAC_Page_Write] = TRUE;
            DAC_Buffer_Count = 0;
            DAC_ToggleWriteDACPage();
            goto Finish;
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
            if(!DAC_ToggleWriteDACPage())
            {
                Nop();
                goto Finish;
            }
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
        if(!FAT_ReadSector(firstSector + j))
        {
            Nop();
            goto Finish;
        }
        while(1)
        {
            if(DAC_ERROR)
            {
                Nop();
                goto Finish;
            }
            if(FileList[file].WAV_DATA.BitsPerSample == 8)
            {
                temp = (SD_Receive_Buffer_Big[SD_Buffer_Count] - 127);
                temp <<= 8;
                DAC_FIFO[DAC_Page_Write][DAC_Buffer_Count] = temp;
            }
            else
            {
                DAC_FIFO[DAC_Page_Write][DAC_Buffer_Count] = (int) MSC_EndianIntArray(&SD_Receive_Buffer_Big[SD_Buffer_Count]);
            }
            SD_Buffer_Count+=FileList[file].WAV_DATA.SampleRepeat;
            DAC_Buffer_Count++;
            SamplesRead ++;
            if(SamplesRead >= FileList[file].WAV_DATA.NumSamples)
            {
                DAC_Buffer_Elements[DAC_Page_Write] = DAC_Buffer_Count;
                DAC_Page_Write_Finished[DAC_Page_Write] = TRUE;
                DAC_Buffer_Count = 0;
                DAC_ToggleWriteDACPage();
                goto Finish;
            }
            if(SD_Buffer_Count >= FAT_BS.bytesPerSector)
            {
                Nop();
                break;
            }
            if(DAC_Buffer_Count >= DAC_BUFFER_SIZE)
            {
                DAC_Buffer_Elements[DAC_Page_Write] = DAC_Buffer_Count;
                DAC_Page_Write_Finished[DAC_Page_Write] = TRUE;
                DAC_Buffer_Count = 0;
                if(!DAC_ToggleWriteDACPage())
                {
                    Nop();
                    goto Finish;
                }
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
            if(!FAT_ReadSector(firstSector + j))
            {
                Nop();
                goto Finish;
            }
            while(1)
            {
                if(DAC_ERROR)
                {
                    Nop();
                    goto Finish;
                }
                if(FileList[file].WAV_DATA.BitsPerSample == 8)
                {
                    temp = (SD_Receive_Buffer_Big[SD_Buffer_Count] - 127);
                    temp <<= 8;
                    DAC_FIFO[DAC_Page_Write][DAC_Buffer_Count] = temp;
                }
                else
                {
                    DAC_FIFO[DAC_Page_Write][DAC_Buffer_Count] = (int) MSC_EndianIntArray(&SD_Receive_Buffer_Big[SD_Buffer_Count]);
                }
                SD_Buffer_Count+=FileList[file].WAV_DATA.SampleRepeat;
                DAC_Buffer_Count++;
                SamplesRead++;
                if(SamplesRead >= FileList[file].WAV_DATA.NumSamples)
                {
                    DAC_Buffer_Elements[DAC_Page_Write] = DAC_Buffer_Count;
                    DAC_Page_Write_Finished[DAC_Page_Write] = TRUE;
                    DAC_Buffer_Count = 0;
                    DAC_ToggleWriteDACPage();
                    goto Finish;
                }
                if(SD_Buffer_Count >= FAT_BS.bytesPerSector)
                {
                    Nop();
                    break;
                }
                if(DAC_Buffer_Count >= DAC_BUFFER_SIZE)
                {
                    DAC_Buffer_Elements[DAC_Page_Write] = DAC_Buffer_Count;
                    DAC_Page_Write_Finished[DAC_Page_Write] = TRUE;
                    DAC_Buffer_Count = 0;
                    if(!DAC_ToggleWriteDACPage())
                    {
                        Nop();
                        goto Finish;
                    }
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

    Finish:
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

/******************************************************************************/
/* WAV_PlayFile_Continuous_Sector
 *
 * This plays the numbered audio file. If the file is not valid then it returns
 * FAIL.
 *
 * The method used to read the SD card is by continuous sector. This means the
 *   sd card is given the address of the first sector int he cluster, then a
 *   continuous read gets all of the data in the cluster. When this method is
 *   used with a large allocation size (64k) there is little to no down time
 *   when reading the data in the cluster.
 *
 * 8-bit samples are stored as unsigned bytes, ranging from 0 to 255.
 * 16-bit samples are stored as 2's-complement signed integers, ranging from
 *   -32768 to 32767.
/******************************************************************************/
unsigned char WAV_PlayFile_Continuous_Sector(unsigned char file)
{
    unsigned long cluster, fileSize, firstSector;
    unsigned char First = TRUE;
    unsigned char WavStatus = PASS;

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
    if(!WAV_MultipleBlockRead(firstSector,firstSector + FAT_BS.sectorPerCluster, file, &First, &WavStatus))
    {
        Nop();
        goto Finish;
    }
    if(WavStatus == FAIL)
    {
        Nop();
        goto Finish;
    }

    /* read the next cluster */
    while(1)
    {
        /* get the next cluster */
        cluster = FAT_GetSetNextCluster (cluster, GET, NULL);
        firstSector = FAT_GetFirstSector (cluster);
        if(!WAV_MultipleBlockRead(firstSector,firstSector + FAT_BS.sectorPerCluster, file, &First, &WavStatus))
        {
            Nop();
            goto Finish;
        }
        if(WavStatus == FAIL || WavStatus == WAV_FINISHED)
        {
            Nop();
            goto Finish;
        }
    }

    Finish:
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

/******************************************************************************/
/* WAV_Continuous_Cluster
 *
 * This function is used by WAV_PlayFile_Continuous_Sector() to play the first
 *  cluster of the wav file.
/******************************************************************************/
unsigned char WAV_Continuous_Cluster(unsigned char file, unsigned char *first, unsigned char *status )
{
    int temp;
    
    if(*first == TRUE)
    {
        /* this is the first sector of the wav file. data starts at byte 44 */
        DAC_ERROR =0; // clear the DAC error
        DAC_Page_Write_Finished[FIRST] = FALSE;
        DAC_Page_Write_Finished[SECOND] = FALSE;
        DAC_Page_Write = FIRST;
        DAC_Page_Read = FIRST;
        DAC_Buffer_Place = 0;
        DAC_Buffer_Count = 0;
        SD_Buffer_Count = 44;
        WAV_DONE = FALSE;
        playing = FALSE;
        *first = FALSE;
        SamplesRead = 0;
    }
    while(1)
    {
        if(DAC_ERROR)
        {
           *status = WAV_FAIL;
           return DONE;
        }
        if(FileList[file].WAV_DATA.BitsPerSample == 8)
        {
            temp = (SD_Receive_Buffer_Big[SD_Buffer_Count] - 127);
            temp <<= 8;
            DAC_FIFO[DAC_Page_Write][DAC_Buffer_Count] = temp;
        }
        else
        {
            DAC_FIFO[DAC_Page_Write][DAC_Buffer_Count] = (int) MSC_EndianIntArray(&SD_Receive_Buffer_Big[SD_Buffer_Count]);
        }
        SD_Buffer_Count+=FileList[file].WAV_DATA.SampleRepeat;
        SamplesRead++;
        DAC_Buffer_Count++;
        if(SamplesRead >= FileList[file].WAV_DATA.NumSamples)
        {
            /* whole file was read */
            DAC_Buffer_Elements[DAC_Page_Write] = DAC_Buffer_Count;
            DAC_Page_Write_Finished[DAC_Page_Write] = TRUE;
            DAC_Buffer_Count = 0;
            DAC_ToggleWriteDACPage();
            *status = WAV_FINISHED;
            return DONE;
        }
        if(SD_Buffer_Count >= FAT_BS.bytesPerSector)
        {
            /* place in wave file */
            SD_Buffer_Count = 0;
            *status = WAV_CONTINUE;
            return NOT_DONE;
        }
        if(DAC_Buffer_Count >= (DAC_BUFFER_SIZE - 2))
        {
            DAC_Buffer_Elements[DAC_Page_Write] = DAC_Buffer_Count;
            DAC_Page_Write_Finished[DAC_Page_Write] = TRUE;
            DAC_Buffer_Count = 0;
            if(!DAC_ToggleWriteDACPage())
            {
                *status = WAV_FAIL;
                return DONE;
            }
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

/******************************************************************************/
/* WAV_MultipleBlockRead
 *
 * Reads SD blocks and stores it in SD_Receive_Buffer_Big. This function is
 *  very similar to SD_readMultipleBlock() except it digestes the Wav
 *  after each read.
 * Command Argument:
 *
 * SDHC and SDXC use the 32-bit argument of memory access commands as block
 * address format. Block length is fixed to 512 bytes regardless CMD16,
 * format. Block SDSC uses the 32-bit argument of memory access commands as
 * byte address length is determined by CMD16,
 * i.e.:
 * (a) Argument 0001h is byte address 0001h in the SDSC and 0001h block in SDHC
 *     and SDXC
 * (b) Argument 0200h is byte address 0200h in the SDSC and 0200h block in SDHC
 *     and SDXC
/******************************************************************************/
unsigned char WAV_MultipleBlockRead(long StartIndex, long StopIndex, unsigned char File, unsigned char *First, unsigned char *Status)
{
    unsigned int i;
    unsigned char* Pbuf = SD_Receive_Buffer_Big;
    unsigned long block;
    unsigned long BytesRead;
    unsigned long BytesReadTotal;
    unsigned long j;
    unsigned char MISOCheck;
    unsigned char ReadStatus = NOT_DONE;

    if(StartIndex > StopIndex)
    {
        return FAIL;
    }
    Global_message.Transmitter = 1;
    Global_message.Command = CMD18;
    block = StartIndex;
    R1_Buffer[0] = 0xFF;
    SD_Clear_SD_Receive_Buffer_Big();

    if(SD_Get_CardType() != SD_CARD_TYPE_SDHC)
    {
        block <<= 9;
    }
    Global_message.Argument = block;

    SD_CMDSPI_send_read(PGlobal_message,R1,R1_Buffer,NO);
    SD_Timeout = 0;
    while(R1_Buffer[0] != 0)
    {
        while(!SPI_WriteRead(0xFF,R1_Buffer,YES))
        {
            if(SD_CardPresent() == FAIL)
            {
                return FAIL;
            }
            SD_Timeout++;
            if(SD_Timeout >= SD_TIMEOUT_MAX)
            {
                SD_CS_INACTIVE();
                return FAIL;
            }
        }
        if(SD_CardPresent() == FAIL)
        {
            return FAIL;
        }
    }

     // wait for data token
    SD_Timeout = 0;
    R1_Buffer[0] = 0xFF;
    while(R1_Buffer[0] != DATA_START_BLOCK)
    {
        while(!SPI_WriteRead(0xFF,R1_Buffer,YES))
        {
            if(SD_CardPresent() == FAIL)
            {
                return FAIL;
            }
            SD_Timeout++;
            if(SD_Timeout >= SD_TIMEOUT_MAX)
            {
                SD_CS_INACTIVE();
                return FAIL;
            }
        }
    }

    // read data
    BytesReadTotal = 0;
    for(j = StartIndex; j < StopIndex; j++)
    {
        /* Check for Card */
        if(SD_CardPresent() == FAIL)
        {
            return FAIL;
        }

        /* read a sector */
        BytesRead = 0;
        SD_Clear_SD_Receive_Buffer_Big();
        Pbuf = &SD_Receive_Buffer_Big[0];
        MISOCheck = YES;
        if(BytesReadTotal != 0)
        {
            for (i=0; i<2; i++)
            {
                if(SPI_WriteRead(0xFF,Pbuf,MISOCheck))
                {
                    BytesReadTotal++;
                }
                MISOCheck = NO;
                Pbuf++;
            }
            Pbuf = &SD_Receive_Buffer_Big[0];
        }
        for (i=0; i<SDblockSize; i++)
        {
            if(SPI_WriteRead(0xFF,Pbuf,MISOCheck))
            {
                BytesRead++;
                BytesReadTotal++;
            }
            MISOCheck = NO;
            Pbuf++;
        }

        // read CRC
        SPIwrite(0xFF); // LSB
        SPIwrite(0xFF); // MSB

        /* function to use the data goes here */
        ReadStatus = WAV_Continuous_Cluster(File, First, Status);
        SD_Clear();
        if(ReadStatus == DONE)
        {
            break;
        }
    }
    SD_CMDSPI_send_read(SD_SetCMD(CMD12,0),R1,R1_Buffer, NO);
    SD_Clear();
    SD_CS_INACTIVE();
    if(ReadStatus == DONE && *Status == WAV_FAIL)
    {
        return FAIL;
    }
    return PASS;
}

/******************************************************************************/
/* WAV_TestAllFiles
 *
 * This function playsback all audio files on the SD card.
/******************************************************************************/
void WAV_TestAllFiles(void)
{
    unsigned char i;
    
    UART_DEBUG_SendStringConstCRLN("WAV File Testing");
    for(i=WaveFilesNumLow; i <=WaveFilesNumHigh; i++)
    {
        if(ValidWAVFiles[i] == PASS)
        {
            UART_DEBUG_SendStringConst("Playing WAV file: ");
            UART_DEBUG_SendStringConstCRLN(&FileList[i].name);
            if(WAV_PlayFile_Random_Sector(i))
            {
                UART_DEBUG_SendStringConstCRLN("Wav played successfully");
            }
            else
            {
                UART_DEBUG_SendStringConstCRLN("Wav failed");
            }
            if(SD_CardPresent() == FAIL)
            {
                SD_State = NOT_INITIALIZED;
                UART_DEBUG_SendStringConstCRLN("SD card removed");
            }
        }
    }

}

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/