#ifndef DISKH
#define DISKH

#include "common.h"
#include "io.h"
#include "terminal.h"

#define ATA1_IO 0x1F0
#define ATA2_IO 0x170
#define ATA1_CR 0x3F6
#define ATA2_CR 0x376

//IO port offsets.
#define IO_DATA 0
#define IO_ERROR 1
#define IO_FEATS 1
#define IO_SECCNT 2
#define IO_SECNUM 3
#define IO_LGAl 3
#define IO_CYLL 4
#define IO_LGAm 4
#define IO_CYLH 5
#define IO_LGAh 5
#define IO_DRIVE 6
#define IO_CMD 7
#define IO_STATUS 7

//Status register bits.
#define S_ERR 0x1
#define S_DRQ 0x8
#define S_SRV 0x10
#define S_DFE 0x20
#define S_RDY 0x40
#define S_BSY 0x80

unsigned int partStart;
unsigned int partEnd;

//Returns false if cannot access disk. True if successful.
bool setupHD();
void hdWrite(void* in, unsigned int loc, unsigned int bytes);
void hdWriteAbs(void* in, unsigned int loc, unsigned int bytes);
void hdRead(void* out, unsigned int loc, unsigned int bytes);
void hdReadAbs(void* out, unsigned int loc, unsigned int bytes);
#endif
