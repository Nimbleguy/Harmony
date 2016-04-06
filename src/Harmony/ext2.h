#ifndef EXT2H
#define EXT2H

#include "common.h"
#include "disk.h"
#include "multiboot.h"
#include "utilities.h"

//Master Boot Record data.
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

struct mbr{
	unsigned char bootstrap[446];
	struct mbrEntry entries[4];
	unsigned char signature[2]; //0x55 0xAA
}__attribute__((packed));

//EXT2 data.
struct superblockExt{ //Only exists if superblock->verMajor >= 1.
	unsigned int nosuInode; //First non-reserved inode. 11 when nonexistant.
	unsigned short inodeSize; //Byte size of inodes. 128 when nonexistant.
	unsigned short blockGroup; //Block group the superblock is part of, if a backup copy.
	unsigned int optional; //Optional features present. 0x1=prealloc, 0x2=afs inodes, 0x4=journaled, 0x8=xattr, 0x10=resizable, 0x20=hashindex.
	unsigned int required; //Required features present. 0x1=compression, 0x2=typed dirs, 0x4=journal replay, 0x8=jounalUsed.
	unsigned int rofeat; //Readonly features present. 0x1=sparse superblock & tables, 0x2=64-bit, 0x4=binary tree.
	unsigned char fsId[16]; //Filesystem ID.
	unsigned char volId[16]; //Volume name, terminated with \0.
	unsigned char path[64]; //Path volume was last mounted to, terminated with \0.
	unsigned int compress; //What compression algorithims used if superblockExt->required & 0x1.
	unsigned char preallocFile; //Blocks to preallocate for files.
	unsigned char preallocDir; //Blocks to preallocate for directories.
	unsigned short reserved1; //Reserved.
	unsigned char journalId[16]; //Same as superblockExt->fsId.
	unsigned int journalInode; //Inode location of the journal.
	unsigned int journalDevice; //Block device of the journal.
	unsigned int orphanInode; //Head of the orphan inode list.
	unsigned char reserved2[788]; //Reserved.
}__attribute__((packed));

struct superblock{
	unsigned int inodes;
	unsigned int blocks;
	unsigned int resBlocks; //Su reserved blocks.
	unsigned int unallocBlocks;
	unsigned int unallocInodes;
	unsigned int superblock; //Superblock block location.
	unsigned int logBlock; //1024 << (x) to get block size.
	unsigned int logFragment; //1024 << (x) to get fragment size.
	unsigned int blockGroupSize; //Blocks in a block group.
	unsigned int fragmentGroupSize; //Fragments in a block group.
	unsigned int inodeGroupSize; //Inodes in a block group.
	unsigned int mountTime; //POSIX last mount time.
	unsigned int writeTime; //POSIX last write time.
	unsigned short fsckLast; //Amount of mount times since last fsck.
	unsigned short fsckMax; //Amount of mounts before fsck must be done.
	unsigned short signature; //0xEF53
	unsigned short fsState; //FS state. 1 is clean; 2 is errored.
	unsigned short errorDo; //What to do when bad things(tm). 1 is ignore, 2 is remount as ro, 3 is kernel panic.
	unsigned short verMinor; //Minor version.
	unsigned int fsckTime; //POSIX last fsck time.
	unsigned int fsckInterval; //POSIX interval between forced fscks.
	unsigned int osId; //ID of creator OS. 0 is linux, 1 is hurd, 2 is masix, 3 is freebsd, 4 is lites.
	unsigned int verMajor; //Major version.
	unsigned short superUser; //UID of su.
	unsigned short superGroup; //GID of su.
	struct superblockExt ext; //Extension of the superblock. See struct superblockExt.
}__attribute__((packed));

struct block{
	unsigned int blockAddr; //Block address of the block usage bitmap.
	unsigned int inodeAddr; //Block address of the inode usage bitmap.
	unsigned int inodeTable; //Block address of inode table.
	unsigned short unallocBlocks; //Unallocated blocks in group.
	unsigned short unallocInodes; //Unallocated inodes in group.
	unsigned short dirs; //Number of directories.
	unsigned char reserved[14]; //Reserved.
}__attribute__((packed));

struct inode{
	unsigned short perms; //inode->perms & 0xF000 is type, everything else is permissions.
	unsigned short uid; //Owner user id.
	unsigned int lowSize; //Lower 32 bits of filesize.
	unsigned int timeAccess; //Last access POSIX time.
	unsigned int timeCreate; //Creation POSIX time.
	unsigned int timeModify; //Last modification POSIX time.
	unsigned int timeDelete; //POSIX time of deletion.
	unsigned short gid; //Owner group id.
	unsigned short dirs; //Amount of directories, or hard links, leading to the inode. When 0, it's unallocated.
	unsigned int sectors; //Disk sectors, not blocks, used. Does not include parents or this structure itself.
	unsigned int flags; //Just look up inode flags for ext2.
	unsigned int os1; //OS specific value #1.
	unsigned int dBlock[12]; //12 block pointers to where data is stored.
	unsigned int iBlock1; //Block pointer to 12 dBlocks.
	unsigned int iBlock2; //Block pointer to as many iBlock1s as possible.
	unsigned int iBlock3; //Block pointer to as many iBlock2s as possible.
	unsigned int gen; //Generation number.
	unsigned int xattr; //In superblock->verMajor >= 1, extended attribute block location. If not, then reserved.
	unsigned int acl; //If superblock->verMajor >= 1, (If file then upper 32 bits of size, if directory then directory xattr), if not then reserved.
	unsigned int fragment; //Block address of fragment.
	unsigned int os2; //OS specific value #2.
}__attribute__((packed));

//True if initialized successfully.
bool setupFS();

#endif
