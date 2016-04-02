#ifndef FATH
#define FATH

#include "common.h"
#include "disk.h"
#include "multiboot.h"
#include "utilities.h"

struct mbr{
	unsigned char bootstrap[446];
	struct mbrEntry entries[4];
	unsigned char signature[2]; //0x55AA. [0] = 0x55, [1] = 0xAA.
}__attribute__((packed));

struct mbrEntry{
        unsigned char bootable;
        unsigned char startHead;
        unsigned char startSect : 6;
        unsigned short startCyl : 10;
        unsigned char id;
        unsigned char endHead;
        unsigned char endSect : 6;
        unsigned short endCyl : 10;
        unsigned int lba;
        unsigned int sectors;
}__attribute__((packed));

struct bootRecord{
        unsigned char code[3];
        unsigned char oem[8];
        //Bios Parameter Block
        unsigned short bps; //Blocks per sector.
        unsigned char spc; //Sectors per cluster.
        unsigned short reservedSectors;
        unsigned char fats;
        unsigned short rootDirs; //FAT32 makes this 0.
        unsigned short sectors; //0 if more than 65535.
        unsigned char descriptor; //F8 if hard disk.
        unsigned short spf; //Sectors per fat. Useless on FAT32.
        unsigned short spt; //Sectors per track. Donut trust.
        unsigned short heads; //Donut trust.
        unsigned int absLBA;
        unsigned int lSectors; //Amount of sectors if bootRecord->sectors is 0.
        //FAT32 Extension
        unsigned int fatSize; //In sectors.
        unsigned short flags;
        unsigned short version;
        unsigned int rootCluster;
        unsigned short infoSector; //Sector number of the fsinfo struct.
        unsigned short backupSector; //Sector of backup boot sector.
        unsigned int zero[3];
        unsigned char drive;
        unsigned char ntFlags; //Useless, we're not NT.
        unsigned char signature; //Must be 0x28 or 0x29
        unsigned int volSerial; //Volume ID serial number.
        unsigned char label[11]; //Volume label. Padded with spaces.
        unsigned char identity[8]; //System identifier string, always "FAT32   ". Donut Trust.
        unsigned char bootCode[420];
        unsigned char bootSignature[2]; //0x55AA. [0] = 0x55, [1] = 0xAA.
}__attribute__((packed));

struct fsInfo{
	unsigned int signature1; //0x41615252
	unsigned char reserved1[480];
	unsigned int signature2; //0x61417272
	unsigned int free; //0xFFFFFFFF if free clusters is unknown.
	unsigned int next; //Where there are free clusters. If 0xFFFFFFFF, then unknown.
	unsigned char reserved2[12];
	unsigned int signature3; //0xAA550000
}__attribute__((packed));

struct mbr* masterBoot;
struct bootRecord* volumeBoot;
struct fsInfo* fileInfo;
unsigned int dataClusters;

//True if initialized successfully.
bool setupFS();

#endif
