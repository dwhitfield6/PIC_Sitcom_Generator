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

#include "user.h"
#include "FAT.h"
#include "SD.h"
#include "MISC.h"

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

    if(SD_Initialized)
    {
        if(FAT_GetBootSectorData())
        {
            FAT_STATUS = VALID;
            if(FAT_DiscoverFiles())
            {
                return PASS;
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
                dir = &Receive_Buffer_Big[i];

                if(dir->name[0] == EMPTY) //indicates end of the file list of the directory
                {
                    if((flag == GET_FILE) || (flag == DELETE))
                    {
                        return FILE_NOT_EXIST;
                    }
                    return FILE_NOT_EXIST;
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
                                SD_writeBlock(firstSector+sector,Receive_Buffer_Big);
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
                        for(j=0; j<8; j++)
                        {
                            FileList[file].name[j] = dir->name[j];
                        }
                        for(j=8; j<11; j++)
                        {
                            FileList[file].type[j-8] = dir->name[j];
                        }
                        if((dir->attrib != 0x10) && (dir->attrib != 0x08))
                        {
                            FileList[file].location[0] = 'F';
                            FileList[file].location[1] = 'I';
                            FileList[file].location[2] = 'L';
                            FileList[file].location[3] = 'E';
                            FileList[file].location[4] = 0;
                            FileList[file].size = dir->fileSize;
                        }
                        else
                        {
                            if(dir->attrib == 0x10)
                            {
                                FileList[file].location[0] = 'D';
                                FileList[file].location[1] = 'I';
                                FileList[file].location[2] = 'R';
                                FileList[file].location[3] = 0;
                                FileList[file].location[4] = 0;
                            }
                            else
                            {
                                FileList[file].location[0] = 'R';
                                FileList[file].location[1] = 'O';
                                FileList[file].location[2] = 'O';
                                FileList[file].location[3] = 'T';
                                FileList[file].location[4] = 0;
                            }
                        }
                        file++;
                        {
                            if(file >= MAX_FILES)
                            {
                                return FILE_TABLE_FILLED;
                            }
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
 * Contents are printed stored in Receive_Buffer_Big[];
/******************************************************************************/
unsigned char FAT_ReadSector(long sector)
{
   return SD_readBlock(sector);
}

/******************************************************************************/
/* FAT_GetBootSectorData
 *
 * Reads a sector on the FAT32 formatted SD card.
 * Contents are printed stored in Receive_Buffer_Big[];
/******************************************************************************/
unsigned char FAT_GetBootSectorData(void)
{
    unsigned long DataSectors;
    unsigned long TotalClusters;

    if(!FAT_ReadSector(0))
    {
        return FAIL;
    }
    if(Receive_Buffer_Big[0] != 0xE9 && Receive_Buffer_Big[0] != 0xEB)
    {
        if(Receive_Buffer_Big[510] == 0x55 && Receive_Buffer_Big[511] == 0xAA)
        {
            /* the signiture is present so we have a valid MBR structure */
            SectorCopy(Receive_Buffer_Big,PFAT_MBR);
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
        if(Receive_Buffer_Big[510] == 0xAA && Receive_Buffer_Big[511] == 0x55)
        {
            return FAIL;
        }
    }
    SectorCopy(Receive_Buffer_Big,PFAT_BS);
    DataSectors = FAT_BS.totalSectors_F32 - FAT_BS.reservedSectorCount - (FAT_BS.numberofFATs * FAT_BS.FATsize_F32);
    TotalClusters = DataSectors / FAT_BS.sectorPerCluster;
    firstDataSector = FAT_BS.hiddenSectors + FAT_BS.reservedSectorCount + (FAT_BS.numberofFATs * FAT_BS.FATsize_F32);

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
    if(!FAT_ReadSector(unusedSectors + 1))
    {
        return FAIL;
    }
    SectorCopy(Receive_Buffer_Big,PFAT_FS);

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
    unsigned long *FATEntryValue;
    unsigned long FATEntrySector;

    /* get sector number of the cluster entry in the FAT */
    FATEntrySector = unusedSectors + FAT_BS.reservedSectorCount + ((ClusterNumber * 4) / FAT_BS.bytesPerSector);
    
    /* get the offset address in that sector number */
    FATEntryOffset = (unsigned int) ((ClusterNumber * 4) % FAT_BS.bytesPerSector);

    /* read the sector into a buffer */
    if(!FAT_ReadSector(FATEntrySector))
    {
        return FAIL;
    }

    /* get the cluster address from the buffer */
    FATEntryValue = ( unsigned long *) &Receive_Buffer_Big[FATEntryOffset];

    if (Get_nSet == GET)
    {
        return ((*FATEntryValue) & 0x0fffffff);
    }

    *FATEntryValue = ClusterEntry;
    SD_writeBlock(FATEntrySector,Receive_Buffer_Big);
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
/* SectorCopy
 *
 * Copies a sector from the buffer to a structure.
/******************************************************************************/
void SectorCopy(unsigned char* from, void* to)
{
    int count = 0;
    while(count <512)
    {
        *(unsigned char*)to = *from;
        from++;
        to++;
        count++;
    }
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/