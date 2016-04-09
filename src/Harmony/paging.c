#include "paging.h"
unsigned int eDir;
unsigned int eTab;

// This thing is a mess.

void mkPgDir(unsigned short index, bool p, bool rw, bool u, bool wc, bool dc, bool a, bool s, unsigned char extra, unsigned int taddr){
	unsigned int tableaddr;
	if(s){
		tableaddr = taddr;
	}
	else{
		unsigned int tableaddrr = (unsigned int)kalloc_al(sizeof(struct pgTabEntry) * 1024);
		for(unsigned short i = 0; i < 1024; i++){
			((unsigned int*)tableaddrr)[i] = eTab;
		}
		tableaddr = getPhysInt(tableaddrr);
	}

	struct pgDirEntry* e = kalloc_al(sizeof(struct pgDirEntry));
	e->present = p;
	e->rw = rw;
	e->usr = u;
	e->wcache = wc;
	e->dcache = dc;
	e->accessed = a;
	e->zero = 0;
	e->size = s;
	e->free = extra;
	e->table = tableaddr >> 12;
	pgDir[index] = *e;
}

void mkPgTab(unsigned short dir, unsigned short index, bool p, bool rw, bool u, bool wc, bool dc, bool a, bool d, bool g, unsigned char extra, unsigned int pageaddr){
	struct pgTabEntry* e = kalloc_al(sizeof(struct pgTabEntry));
        e->present = p;
        e->rw = rw;
        e->usr = u;
        e->wcache = wc;
        e->dcache = dc;
        e->accessed = a;
	e->dirty = d;
        e->zero = 0;
        e->global = g;
        e->free = extra;
        e->page = pageaddr >> 12;

	struct pgDirEntry* dire = (struct pgDirEntry*)(&(pgDir[dir]));
	struct pgTabEntry* table = (struct pgTabEntry*)getPhysInt((unsigned int)(dire->table) << 12);
	table[index] = *e;
}

void mapPage(unsigned int phys, unsigned int virt, bool user, bool size){
	unsigned short dindex = GETDIR(virt);
	unsigned short tindex = GETTAB(virt);
	if(size){
		mkPgDir(dindex, true, true, user, false, false, false, size, 0, phys & 0xFFF00000);
	}
	else{
		if(!(pgDir[dindex].present) || (pgDir[dindex].present && pgDir[dindex].size)){
			mkPgDir(dindex, true, true, user, false, false, false, size, 0, 0);
		}
		mkPgTab(dindex, tindex, true, true, user, false, false, false, false, false, 0, phys & 0xFFFFF000);
	}
	invlpgb();
}

void enablePaging(){
	if(!pagEnabled){
		unsigned short dstart = GETDIR(0xC0000000);
		unsigned short tstart = GETTAB(0xC0000000);

		fbWrite(its(dstart), DTCOLOR, BLACK);
		fbWrite("\n", DTCOLOR, BLACK);
		fbWrite(its(tstart), DTCOLOR, BLACK);
		fbWrite("\n", DTCOLOR, BLACK);

		pgDir = (struct pgDirEntry*)kalloc_al(sizeof(struct pgDirEntry) * 1024);
		pgDirT = (struct pgDirEntry*)kalloc_al(sizeof(struct pgDirEntry) * 1024);
		//Clear page directory.
		struct pgDirEntry* blankDir = (struct pgDirEntry*)kalloc_al(sizeof(struct pgDirEntry));
		blankDir->present = 0;
		blankDir->rw = 0;
		blankDir->usr = 0;
		blankDir->wcache = 0;
		blankDir->dcache = 0;
		blankDir->accessed = 0;
		blankDir->zero = 0;
		blankDir->size = 0;
		blankDir->free = 0;
		blankDir->table = 0;

		eDir = *((unsigned int*)blankDir);

		unsigned short i;
		for(i = 0; i < 1024; i++){
			pgDir[i] = *blankDir;
		}

		//Setup blank page tables.
		struct pgTabEntry* blankTab = (struct pgTabEntry*)kalloc_al(sizeof(struct pgTabEntry));
	        blankTab->present = 0;
	        blankTab->rw = 0;
		blankTab->usr = 0;
		blankTab->wcache = 0;
		blankTab->dcache = 0;
		blankTab->accessed = 0;
		blankTab->dirty = 0;
		blankTab->zero = 0;
		blankTab->global = 0;
		blankTab->free = 0;
		blankTab->page = 0;

		eTab = *((unsigned int*)blankTab);

		//Higher Half Page Directory, GRUB, BIOS, and Kernel.
		mkPgDir(dstart, true, true, false, false, false, false, true, 0, 0x0);
		//Map heap.
		mkPgDir(dstart + 1, true, true, true, false, false, false, true, 0, 0x400000);
		//Recursive page directory.
		mkPgDir(1024 - 1, true, true, true, false, false, false, false, 0, getPhysInt(pgDir));

		//Enable paging.
		switchPage();
		lpagb(getPhysInt(pgDirT));
		pagEnabled = true;
	}
	else{
		unsigned short i;
		//Higher Half Page Directory, GRUB, BIOS, and Kernel.
		for(i = 0; i < 1024; i++){
//			mapPage(0, 0xC0000000, false, false);
		}
		//Map heap.
		for(i = 0; i < 1024; i++){
			mapPage(0x400000, 0xC0400000, true, false);
		}
		switchPage();
	}
}

//Get physical address.
void* getPhys(void* virt){
	if(pagEnabled){
		unsigned int pdIndex = (unsigned int)virt >> 22;
		unsigned int ptIndex = (unsigned int)virt >> 12 & 0x03FF;
		//Use recursive map to get phys address.
		unsigned int* pt = (unsigned int*)0xFFC00000 + (0x400 * pdIndex);
		return (void*)((pt[ptIndex] & ~0xFFF) + ((unsigned int)virt & 0xFFF));
	}
	else{
		return virt - 0xC0000000;
	}
}

//Set permanent to tmp.
void switchPage(){
	struct pgDirEntry* temp = pgDirT;
	pgDirT = pgDir;
	pgDir = temp;
	memcpy(pgDir, pgDirT, sizeof(struct pgDirEntry) * 1024);
}
