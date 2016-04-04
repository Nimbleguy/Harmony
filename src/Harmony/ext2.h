#ifndef EXT2H
#define EXT2H

#include "common.h"
#include "disk.h"
#include "multiboot.h"
#include "utilities.h"

struct superblock{

}__attribute__((packed));

struct superblockExt{

}__attribute__((packed));

//True if initialized successfully.
bool setupFS();

#endif
