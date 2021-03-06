/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 05/28/15     1.0_DW0a    Initial coding.
 * /******************************************************************************/

/******************************************************************************/
/* Contains Functions for the FAT file system.
 *
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
#include <stdio.h>         /* For sprintf definition */

#include "USER.h"
#include "FAT.h"
#include "SD.h"
#include "MISC.h"
#include "WAV.h"
#include "DAC.h"

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
unsigned char FAT_STATUS = INVALID;
MBR FAT_MBR;
MBR *PFAT_MBR;
BS  FAT_BS;
BS *PFAT_BS;
PARTITION FAT_PART1;
PARTITION *PFAT_PART1;
FS FAT_FS;
FS *PFAT_FS;
unsigned long firstDataSector = 0;
unsigned char freeClusterCountUpdated;
unsigned long unusedSectors;
FAT_FILE FileList[MAX_FILES];
unsigned char RootName[9];
#ifndef LOW_MEMORY
unsigned char Cluster[MAXsectorPerCluster][MAXbytesPerSector];
#endif
unsigned char SD_NumFiles = 0;
unsigned long fatStartBlock;

/******************************************************************************/
/* Inline Functions
/******************************************************************************/

/******************************************************************************/
/* Functions
/******************************************************************************/

/******************************************************************************/
/* InitFAT
 *
 * Initializes the File system.
/******************************************************************************/
unsigned char InitFAT(void)
{
    PFAT_MBR = &FAT_MBR;
    PFAT_BS = &FAT_BS;
    PFAT_PART1 = &FAT_PART1;
    PFAT_FS = &FAT_FS;

    if(SD_State == INITIALIZED || SD_State == RUNNING)
    {
        if(FAT_GetBootSectorData())
        {
            FAT_STATUS = VALID;
            if(FAT_DiscoverFiles())
            {
                if(WAV_CheckFiles())
                {
                    return PASS;
                }
                return FAIL;
            }
        }
    }
    return FAIL;
}

/******************************************************************************/
/* InitFAT
 *
 * Searches the SD card looking for files.
/******************************************************************************/
unsigned char FAT_DiscoverFiles(void)
{
    if(FAT_FindFiles(GET_LIST,NULL,NULL) == FILE_TABLE_FILLED)
    {
        return PASS;
    }
    return FAIL;
}

/******************************************************************************/
/* FAT_ConvertFileName
 *
 * This function converts the normal short file name into FAT format.
/******************************************************************************/
unsigned char FAT_ConvertFileName(unsigned char *fileName)
{
    unsigned char fileNameFAT[11];
    unsigned char j, k;
    for (j=0; j<12; j++)
    {
        if (fileName[j] == '.' )
        {
            break ;
        }
    }

    if (j>8)
    {
        return FAIL;
    }

    for (k=0; k<j; k++) //setting file name
    {
        fileNameFAT[k] = fileName[k];
    }
    
    for (k=j; k<=7; k++) //filling file name trail with blanks
    {
        fileNameFAT[k] = ' ' ;
    }

    j++;
    for (k=8; k<11; k++) //setting file extention
    {
        if (fileName[j] != 0)
        {
            fileNameFAT[k] = fileName[j++];
        }
        else //filling extension trail with blanks
        {
            while (k<11)
            {
                fileNameFAT[k++] = ' ' ;
            }
        }
    }

    for (j=0; j<11; j++) //converting small letters to caps
    {
        MSC_LowercaseChar(&fileNameFAT[j]);
    }
    for (j=0; j<11; j++)
    {
        fileName[j] = fileNameFAT[j];
    }
    return PASS;
}

/******************************************************************************/
/* FAT_FindFiles
 *
 * This function gets the directory or the file list or a single file address
 *  (cluster number) or can delete a specified file.
 *
 * flag: GET_LIST, GET_FILE, or DELETE
/******************************************************************************/
unsigned char FAT_FindFiles (unsigned char flag, unsigned char *fileName, PDIR dirGood)
{
    unsigned long cluster, sector, firstSector, firstCluster, nextCluster;
    PDIR dir;
    unsigned int i;
    unsigned char j;
    unsigned char file = 0;
    unsigned char FilesFound = FALSE;

    cluster = FAT_BS.rootCluster; //root cluster

    while(1)
    {
        firstSector = FAT_GetFirstSector(cluster);

        for(sector = 0; sector < FAT_BS.sectorPerCluster; sector++)
        {
            if(!FAT_ReadSector(firstSector + sector))
            {
                return READ_ERROR;
            }

            for(i=0; i<FAT_BS.bytesPerSector; i+=32)
            {
                dir = &SD_Receive_Buffer_Big[i];

                if(dir->name[0] == EMPTY) //indicates end of the file list of the directory
                {
                    if((flag == GET_FILE) || (flag == DELETE))
                    {
                        return FILE_NOT_EXIST;
                    }
                    else
                    {
                        if(FilesFound)
                        {
                            return FILE_TABLE_FILLED;
                        }
                        else
                        {
                            return NO_FILES;
                        }
                    }
                }
                if((dir->name[0] != DELETED) && (dir->attrib != ATTR_LONG_NAME))
                {
                    if((flag == GET_FILE) || (flag == DELETE))
                    {
                        for(j=0; j<11; j++)
                        {
                            if(dir->name[j] != fileName[j])
                            {
                                break;
                            }
                        }
                        if(j == 11)
                        {
                            if(flag == GET_FILE)
                            {
                                dir = dirGood;
                                return FILE_FOUND;
                            }
                            else
                            {
                                /* Deleting sequence */
                                firstCluster = (((unsigned long) dir->firstClusterHI) << 16) | dir->firstClusterLO;

                                /* mark file as 'deleted' in FAT table */
                                dir->name[0] = DELETED;
                                SD_writeBlock(firstSector+sector,SD_Receive_Buffer_Big);
                                //FAT_FreeMemoryUpdate (ADD, dir->fileSize);

                                /* update next free cluster entry in FSinfo sector */
                                cluster = FAT_GetSetFreeCluster (NEXT_FREE, GET, 0);
                                if(firstCluster < cluster)
                                {
                                    FAT_GetSetFreeCluster (NEXT_FREE, SET, firstCluster);
                                }

                                /* mark all the clusters allocated to the file as 'free' */
                                while(1)
                                {
                                    nextCluster = FAT_GetSetNextCluster (firstCluster, GET, 0);
                                    FAT_GetSetNextCluster (firstCluster, SET, 0);
                                    if(nextCluster > 0x0ffffff6)
                                    {
                                        return FILE_DELETED;
                                    }
                                    firstCluster = nextCluster;
                                }
                            }
                        }
                    }
                    else
                    {
                        /* GET_LIST */
                        if((dir->attrib != 0x10) && (dir->attrib != 0x08))
                        {
                            FilesFound = TRUE;
                            for(j=0; j<8; j++)
                            {
                                FileList[file].name[j] = dir->name[j];
                            }
                            for(j=8; j<11; j++)
                            {
                                FileList[file].extention[j-8] = dir->name[j];
                            }
                            FileList[file].type[0] = 'F';
                            FileList[file].type[1] = 'I';
                            FileList[file].type[2] = 'L';
                            FileList[file].type[3] = 'E';
                            FileList[file].type[4] = 0;
                            FileList[file].size = dir->fileSize;
                            FileList[file].firstCluster =(((unsigned long) dir->firstClusterHI) << 16) | dir->firstClusterLO;
                            file++;
                            SD_NumFiles = file;
                        }
                        else
                        {
                            if(dir->attrib == 0x10)
                            {
                                /* Directory (folder) */
                            }
                            else
                            {
                                /* Root */
                                for(j=0; j<8; j++)
                                {
                                    RootName[j] = dir->name[j];
                                }
                                RootName[9] = 0;
                            }
                        }                      
                        if(file >= MAX_FILES)
                        {
                            return FILE_TABLE_FILLED;
                        }
                    }
                }
            }
        }

        cluster = (FAT_GetSetNextCluster (cluster, GET, 0));

        if(cluster > 0x0ffffff6)
        {
            return SEARCH_SUCCESS;
        }
        if(cluster == 0)
        {
            return ERROR_CLUSTER;
        }
    }
    return SEARCH_SUCCESS;
}

/******************************************************************************/
/* FAT_ReadSector
 *
 * Reads a sector on the FAT32 formatted SD card.
 * Contents are printed stored in SD_Receive_Buffer_Big[];
/******************************************************************************/
unsigned char FAT_ReadSector(long sector)
{
   return SD_readBlock(sector);
}

/******************************************************************************/
/* FAT_GetBootSectorData
 *
 * Reads a sector on the FAT32 formatted SD card.
 * Contents are printed stored in SD_Receive_Buffer_Big[];
/******************************************************************************/
unsigned char FAT_GetBootSectorData(void)
{
    unsigned long DataSectors;
    unsigned long TotalClusters;

    if(!FAT_ReadSector(0))
    {
        return FAIL;
    }
    
    FAT_PART1.firstSector = 0;
    if(SD_Receive_Buffer_Big[0] != 0xE9 && SD_Receive_Buffer_Big[0] != 0xEB)
    {
        if(SD_Receive_Buffer_Big[510] == 0x55 && SD_Receive_Buffer_Big[511] == 0xAA)
        {
            /* the signiture is present so we have a valid MBR structure */
            FAT_SectorCopy(SD_Receive_Buffer_Big,PFAT_MBR);
            FAT_Copy(&FAT_MBR.partitionData, &FAT_PART1, 64);
        }
        else
        {
            return FAIL;
        }
        //VoidBufferCopy(FAT_MBR.partitionData, PFAT_PART1, PART_SIZE);
        if(!SD_readBlock(FAT_PART1.firstSector))
        {
            return FAIL;
        }
        if(SD_Receive_Buffer_Big[510] == 0xAA && SD_Receive_Buffer_Big[511] == 0x55)
        {
            return FAIL;
        }
    }
    FAT_SectorCopy(SD_Receive_Buffer_Big,PFAT_BS);
    DataSectors = FAT_BS.totalSectors_F32 - FAT_BS.reservedSectorCount - (FAT_BS.numberofFATs * FAT_BS.FATsize_F32);
    TotalClusters = DataSectors / FAT_BS.sectorPerCluster;
    firstDataSector = FAT_BS.hiddenSectors + FAT_BS.reservedSectorCount + (FAT_BS.numberofFATs * FAT_BS.FATsize_F32);
    fatStartBlock = FAT_PART1.firstSector + FAT_BS.reservedSectorCount;

    if(FAT_BS.bytesPerSector > MAXbytesPerSector)
    {
        return FAIL;
    }
    if(FAT_BS.sectorPerCluster > MAXsectorPerCluster)
    {
        return FAIL;
    }
    if ((FAT_GetSetFreeCluster(TOTAL_FREE, GET, 0)) > TotalClusters) //check if FSinfo free clusters count is valid
    {
        freeClusterCountUpdated = FALSE;
    }
    else
    {
        freeClusterCountUpdated = TRUE;
    }
    return PASS;
}
/******************************************************************************/
/* FAT_GetSetFreeCluster
 *
 * Gets or sets the next free cluster or the total free clusters in FSinfo
 * sector of SD card.
/******************************************************************************/
unsigned long FAT_GetSetFreeCluster(unsigned char Total_nNext, unsigned char Get_nSet, unsigned long FSEntry)
{
    if(!FAT_ReadSector(1))
    {
        return FAIL;
    }
    FAT_SectorCopy(SD_Receive_Buffer_Big,PFAT_FS);

    if (FAT_FS.leadSignature != 0x41615252 && FAT_FS.leadSignature != 0x61417272 && FAT_FS.leadSignature != 0xaa550000)
    {
        return 0xffffffff;
    }
    if (Get_nSet == GET)
    {
        if(Total_nNext == TOTAL_FREE)
        {
            return FAT_FS.freeClusterCount;
        }
        else
        {
            return (FAT_FS.nextFreeCluster);
        }
    }
    else
    {
        if(Total_nNext == TOTAL_FREE)
        {
            FAT_FS.freeClusterCount = FSEntry;
        }
        else
        {
            FAT_FS.nextFreeCluster = FSEntry;
        }
        SD_writeBlock(unusedSectors + 1,PFAT_FS);
    }
    return 0xFFFFFFFF;
}
/******************************************************************************/
/* FAT_GetSetNextCluster
 *
 * Gets the cluster entry value from FAT to find out the next cluster in
 * the chain or sets the  new cluster entry in FAT.
/******************************************************************************/
unsigned long FAT_GetSetNextCluster(unsigned long ClusterNumber, unsigned char Get_nSet, unsigned long ClusterEntry)
{
    unsigned int FATEntryOffset;
    unsigned long FATEntryValue;
    unsigned long FATEntrySector;
    unsigned long temp1,temp2,temp3,temp4;

    /* get sector number of the cluster entry in the FAT */
    FATEntrySector = fatStartBlock + ((ClusterNumber * 4) / FAT_BS.bytesPerSector);
    
    /* get the offset address in that sector number */
    FATEntryOffset = (unsigned int) ((ClusterNumber * 4) % FAT_BS.bytesPerSector);

    /* read the sector into a buffer */
    if(!FAT_ReadSector(FATEntrySector))
    {
        return FAIL;
    }

    /* get the cluster address from the buffer */
    temp1 = SD_Receive_Buffer_Big[FATEntryOffset];
    temp2 = SD_Receive_Buffer_Big[FATEntryOffset + 1];
    temp3 = SD_Receive_Buffer_Big[FATEntryOffset + 2];
    temp4 = SD_Receive_Buffer_Big[FATEntryOffset + 3];
    FATEntryValue = temp1 + (temp2 << 8) + (temp3 << 16) + (temp4 << 24);

    if (Get_nSet == GET)
    {
        return (FATEntryValue & 0x0fffffff);
    }

    SD_Receive_Buffer_Big[FATEntryOffset] = ClusterEntry;
    SD_writeBlock(FATEntrySector,SD_Receive_Buffer_Big);
    return PASS;
}

/******************************************************************************/
/* FAT_GetFirstSector
 *
 * Calculates the first sector address of any given cluster
/******************************************************************************/
unsigned long FAT_GetFirstSector(unsigned long clusterNumber)
{
    return ((clusterNumber - 2) * FAT_BS.sectorPerCluster) + firstDataSector;
}

/******************************************************************************/
/* FAT_SectorCopy
 *
 * Copies a sector from the buffer to a structure.
/******************************************************************************/
void FAT_SectorCopy(unsigned char* from, void* to)
{
    int count = 0;
    while(count <MAXbytesPerSector)
    {
        *(unsigned char*)to = *from;
        from++;
        to++;
        count++;
    }
}

/******************************************************************************/
/* FAT_Copy
 *
 * Copies a part of a sector from the buffer to a structure.
/******************************************************************************/
void FAT_Copy(unsigned char* from, void* to, unsigned int amount)
{
    int count = 0;
    while(count <amount)
    {
        *(unsigned char*)to = *from;
        from++;
        to++;
        count++;
    }
}

/******************************************************************************/
/* ReadCluster
 *
 * Copies a sector from the buffer to a structure.
/******************************************************************************/
#ifndef LOW_MEMORY
unsigned char FAT_ReadCluster(unsigned long cluster)
{
    unsigned long firstSector, sector;

    firstSector = FAT_GetFirstSector(cluster);

    for(sector = 0; sector < FAT_BS.sectorPerCluster; sector++)
    {
        if(!FAT_ReadSector(firstSector + sector))
        {
            return FAIL;
        }
        FAT_SectorCopy(SD_Receive_Buffer_Big,&Cluster[sector][0]);
    }
    return PASS;
}
#endif
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/