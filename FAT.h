/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 05/28/15     1.0_DW0a    Initial coding.
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#ifndef FAT_H
#define	FAT_H

#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#include "WAV.h"
#include "user.h"

/******************************************************************************/
/* Structures                                                                 */
/******************************************************************************/
//Structure to access Master Boot Record for getting info about partioions
typedef struct __attribute__((packed)) MBR_Struct
{
    unsigned char     nothing[446];      //ignore, placed here to fill the gap in the structure
    unsigned char     partitionData[64]; //partition records (16x4)
    unsigned int      signature;         //0xaa55
}MBR;

typedef struct __attribute__((packed)) PartitionInfo_Struct
{
    unsigned char     status;                 //0x80 - active partition
    unsigned char     headStart;              //starting head
    unsigned int      cylSectStart;           //starting cylinder and sector
    unsigned char     type;                   //partition type
    unsigned char     headEnd;                //ending head of the partition
    unsigned int      cylSectEnd;             //ending cylinder and sector
    unsigned long     firstSector;            //total sectors between MBR & the first sector of the partition
    unsigned long     sectorsTotal;           //size of this partition in sectors
}PARTITION;

//Structure to access boot sector data
typedef struct __attribute__((packed)) BS_Struct
{
    unsigned char jumpBoot[3]; 	 //default: 0x009000EB
    unsigned char OEMName[8];
    unsigned int  bytesPerSector;   //default: 512
    unsigned char sectorPerCluster;
    unsigned int  reservedSectorCount;
    unsigned char numberofFATs;
    unsigned int  rootEntryCount;
    unsigned int  totalSectors_F16; //must be 0 for FAT32
    unsigned char mediaType;
    unsigned int  FATsize_F16; 	 //must be 0 for FAT32
    unsigned int  sectorsPerTrack;
    unsigned int  numberofHeads;
    unsigned long hiddenSectors;
    unsigned long totalSectors_F32;
    unsigned long FATsize_F32; 	 //count of sectors occupied by one FAT
    unsigned int  extFlags;
    unsigned int  FSversion; 	   //0x0000 (defines version 0.0)
    unsigned long rootCluster;      //first cluster of root directory (=2)
    unsigned int  FSinfo; 		  //sector number of FSinfo structure (=1)
    unsigned int  BackupBootSector;
    unsigned char reserved[12];
    unsigned char driveNumber;
    unsigned char reserved1;
    unsigned char bootSignature;
    unsigned long volumeID;
    unsigned char volumeLabel[11];   //"NO NAME "
    unsigned char fileSystemType[8]; //"FAT32"
    unsigned char bootData[420];
    unsigned int  bootEndSignature;  //0xaa55
}BS;

//Structure to access FSinfo sector data
typedef struct __attribute__((packed)) FSInfo_Struct
{
    unsigned long leadSignature; 	 //0x41615252
    unsigned char reserved1[480];
    unsigned long structureSignature; //0x61417272
    unsigned long freeClusterCount;   //initial: 0xffffffff
    unsigned long nextFreeCluster;    //initial: 0xffffffff
    unsigned char reserved2[12];
    unsigned long trailSignature; 	//0xaa550000
}FS;

//Structure to access Directory Entry in the FAT
typedef struct __attribute__((packed)) DIR_Struct
{
    unsigned char name[11];
    unsigned char attrib; 		//file attributes
    unsigned char NTreserved; 	//always 0
    unsigned char timeTenth; 	 //tenths of seconds, set to 0 here
    unsigned int  createTime;     //time file was created
    unsigned int  createDate;     //date file was created
    unsigned int  lastAccessDate;
    unsigned int  firstClusterHI; //higher word of the first cluster number
    unsigned int  writeTime; 	 //time of last write
    unsigned int  writeDate; 	 //date of last write
    unsigned int  firstClusterLO; //lower word of the first cluster number
    unsigned long fileSize; 	  //size of file in bytes
}DIR, *PDIR;

// wav file info
typedef struct __attribute__((packed)) WAV_Struct
{
    unsigned char valid;
    unsigned long ChunkSize;
    unsigned long Subchunk1Size;
    unsigned int  AudioFormat;
    unsigned int  NumChannels;
    unsigned long SampleRate;
    unsigned long ByteRate;
    unsigned char BitsPerSample;
    unsigned long Subchunk2Size;
    unsigned long NumSamples;
}WAV;

// File structure
typedef struct __attribute__((packed)) File_Struct
{
    unsigned char name[9];
    unsigned char extention[4];
    unsigned char type[5];
    unsigned long size;
    unsigned long firstCluster;
    WAV           WAV_DATA;
}FAT_FILE, *PFAT_FILE;

/******************************************************************************/
/* MAXsectorPerCluster
 *
 * This is the maximume sectors (512 bytes) per cluster.
/******************************************************************************/
#define MAXsectorPerCluster 16

/******************************************************************************/
/* MAXsectorPerCluster
 *
 * This is the maximume sectors (512 bytes) per cluster.
/******************************************************************************/
#define MAXbytesPerSector 512

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/
//Attribute definitions for file/directory
#define ATTR_READ_ONLY      0x01
#define ATTR_HIDDEN         0x02
#define ATTR_SYSTEM         0x04
#define ATTR_VOLUME_ID      0x08
#define ATTR_DIRECTORY      0x10
#define ATTR_ARCHIVE        0x20
#define ATTR_LONG_NAME      0x0f


#define DIR_ENTRY_SIZE      0x32
#define EMPTY               0x00
#define DELETED             0xe5
#define GET                 0
#define SET                 1
#define READ                0
#define VERIFY              1
#define ADD                 0
#define REMOVE              1
#define TOTAL_FREE          1
#define NEXT_FREE           2
#define GET_LIST            0
#define GET_FILE            1
#define DELETE              2
#define FAT_EOF             0x0fffffff
#define PART_SIZE           64
#define INVALID             0
#define VALID               1

#define READ_ERROR          0
#define FILE_NOT_EXIST      1
#define FILE_FOUND          2
#define FILE_DELETED        3
#define ERROR_CLUSTER       4
#define SEARCH_SUCCESS      5
#define FILE_TABLE_FILLED   6
#define NO_FILES            7


/******************************************************************************/
/* Macro Functions                                                            */
/******************************************************************************/

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
extern unsigned char FAT_STATUS;
extern FAT_FILE FileList[MAX_FILES];
extern MBR FAT_MBR;
extern BS  FAT_BS;
extern PARTITION FAT_PART1;
extern FS FAT_FS;

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/
unsigned char FAT_ReadSector(long sector);
unsigned char FAT_GetBootSectorData(void);
void FAT_SectorCopy(unsigned char* from, void* to);
unsigned char InitFAT(void);
unsigned long FAT_GetSetFreeCluster(unsigned char Total_nNext, unsigned char Get_nSet, unsigned long FSEntry);
unsigned long FAT_GetSetNextCluster(unsigned long ClusterNumber, unsigned char Get_nSet, unsigned long ClusterEntry);
unsigned long FAT_GetFirstSector(unsigned long clusterNumber);
unsigned char FAT_FindFiles (unsigned char flag, unsigned char *fileName, PDIR dirGood);
unsigned char FAT_DiscoverFiles(void);
unsigned char FAT_ReadCluster(unsigned long cluster);
unsigned char FAT_ConvertFileName(unsigned char *fileName);

#endif	/* FAT_H */