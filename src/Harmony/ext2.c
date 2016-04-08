#include "ext2.h"

//True if successful.
bool setupFS(){
	struct mbr* masterBoot = (struct mbr*)malloc(sizeof(struct mbr));
	hdReadAbs(masterBoot, 0x0, sizeof(struct mbr)); //Read the MBR from abs sector 1.

	if(!((masterBoot->signature)[0] == 0x55 && (masterBoot->signature)[1] == 0xAA)){
		fbWrite(hts((unsigned int)masterBoot), RED, BLACK);
		fbWrite(" Invalid MBR!\n", RED, BLACK);
		free(masterBoot);
		return false;
	}

	unsigned char part = (multiboot->boot).part1;

	if(!(multiboot->flags & 0x1) || part > 4){
		//Retrieve root device.
		loopf(); //temp.
	}

	struct mbrEntry entry = (masterBoot->entries)[part];
	partStart = entry.lba;
	partEnd = partStart + entry.sectors;

	struct superblock* sb = (struct superblock*)malloc(sizeof(struct superblock));
	hdRead(sb, 1, sizeof(struct superblock)); //Read the superblock from offset 1024 bytes, which is 1 in LBA.

	if(!(sb->signature == 0xEF53)){
		fbWrite("Invalid Superblock!\n", RED, BLACK);
		free(masterBoot);
		free(sb);
		return false;
	}
	free(masterBoot);
	free(sb);
	return true;
}
