#include "fat.h"

//True if successful.
bool setupFS(){
	//First get info from MBR.
	struct mbr* mas = (struct mbr*)malloc(sizeof(struct mbr));
	hdReadAbs(masterBoot, 0, sizeof(struct mbr)); //Read the MBR on absolute sector 1.

	if(!((mas->signature)[0] == 0x55 && (mas->signature)[1] = 0xAA)){
		fbWrite("No valid MBR! Signature = ", RED, BLACK);
		fbWrite(hts(*signature), RED, BLACK);
		fbWrite("\n", RED, BLACK);
		free(mas);
		return false;
	}

	if(!(multiboot->flags & 0x1)){
		fbWrite("Root device unable to be retrieved! MB->FLAG & 0x1!\n", RED, BLACK);
		free(mas);
		return false;
	}

	unsigned char part = (multiboot->boot).part1;

	if(part > 4){
		//Find it manually somehow.
	}

	struct mbrEntry = (mbr->entries)[part];
	partStart = mbrEntry.lba;
	partEnd = partStart + mbrEntry.sectors;

	struct bootRecord* rec = (struct bootRecord*)malloc(sizeof(struct bootRecord));
	hdRead(rec, 0, sizeof(struct bootRecord); //Read the Volume Boot Record from relative sector 1.

	if(rec->sectors > mas->sectors || rec->lSectors > mas->sectors){
		fbWrite("FAT has more sectors than possible!\n", RED, BLACK);
		free(rec);
		free(mas);
		return false;
	}

	struct fsInfo* info = (struct fsInfo*)malloc(sizeof(struct fsInfo));
	hdRead(info, rec->infoSector, sizeof(struct fsInfo));

	if(!(info->signature1 == 0x41615252 && info->signature2 == 0x61417272 && info->signature3 == 0xAA550000)){
		fbWrite("FSINFO has invalid signature!\n", RED, BLACK);
		free(rec);
		free(mas);
		free(info);
		return false;
	}

	dataClusters = ((rec->sectors + rec->lSectors) - (rec->reservedSectors + (rec->fats * rec->fatSize))) / rec->spc;

	if(dataClusters < 65525){ //Docs say this checks if FAT12 or FAT16.
		fbWrite("Unknown FAT!\n", RED, BLACK);
		free(rec);
		free(mas);
		free(info);
		return false;
	}

	//Make permanent.
	masterBoot = kalloc(sizeof(struct mbr));
	memmove(masterBoot, mas, sizeof(struct mbr));

	volumeBoot = kalloc(sizeof(struct bootRecord));
	memmove(volumeBoot, rec, sizeof(struct bootRecord));

	fileInfo = kalloc(sizeof(struct fsInfo));
	memmove(fileInfo, info, sizeof(struct fsInfo));

	free(mas);
	free(rec);
	free(info);
	return true;
}
