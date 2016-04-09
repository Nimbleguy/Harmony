#include "paging.h"
unsigned int eDir;
unsigned int eTab;

// This thing is a mess.

void mkPgDir(unsigned short index, bool p, bool rw, bool u, bool wc, bool dc, bool a, bool s, unsigned char extra, unsigned int taddr){
	unsigned int tableaddr;
	unsigned short i;
	if(s){
		tableaddr = taddr;
	}
	else if(taddr == 0){
		unsigned int tableaddrr = (unsigned int)kalloc_al(sizeof(struct pgTabEntry) * 1024);
		for(i = 0; i < 1024; i++){
			((unsigned int*)tableaddrr)[i] = eTab;
		}
		tableaddr = getPhysInt(tableaddrr);
	}
	else{
		tableaddr = taddr;
	}

	struct pgDirEntry e = {0};
	e.present = p;
	e.rw = rw;
	e.usr = u;
	e.wcache = wc;
	e.dcache = dc;
	e.accessed = a;
	e.zero = 0;
	e.size = s;
	e.free = extra;
	e.table = tableaddr >> 12;
	pgDir[index] = e;
}

void mkPgTab(unsigned short dir, unsigned short index, bool p, bool rw, bool u, bool wc, bool dc, bool a, bool d, bool g, unsigned char extra, unsigned int pageaddr){
	struct pgTabEntry e = {0};
        e.present = p;
        e.rw = rw;
        e.usr = u;
        e.wcache = wc;
        e.dcache = dc;
        e.accessed = a;
	e.dirty = d;
        e.zero = 0;
        e.global = g;
        e.free = extra;
        e.page = pageaddr >> 12;

	if(pagEnabled){
		struct pgTabEntry* table = (struct pgTabEntry*)(0xFFC00000 + (0x1000 * dir));
		table[index] = e;
	}
	else{
		struct pgDirEntry* dire = (struct pgDirEntry*)(&(pgDir[dir]));
		struct pgTabEntry* table = (struct pgTabEntry*)(((unsigned int)(dire->table) << 12) + 0xC0000000);
		table[index] = e;
	}
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
		pgDir = (struct pgDirEntry*)kalloc_al(sizeof(struct pgDirEntry) * 1024);
		//Clear page directory and setup blanks.
		struct pgDirEntry blankDir = {0};

		eDir = 0;
		eTab = 0;

		unsigned short i;
		for(i = 0; i < 1024; i++){
			pgDir[i] = blankDir;
		}

		//Recursive map.
		mkPgDir(1024 - 1, true, true, false, false, false, false, false, 0, getPhysInt(pgDir));
		invlpgb();
		//Higher Half Page Directory, GRUB, BIOS, and Kernel.
		for(i = 0; i < 1024; i++){
			mapPage(0 + (i * 0x1000), 0xC0000000 + (i * 0x1000), false, false);
		}
		//mkPgDir(dstart, true, true, false, false, false, false, true, 0, 0x0);
		//Kernel heap.
		for(i = 0; i < 1024; i++){
			mapPage(0x400000 + (i * 0x1000), 0xC0400000 + (i * 0x1000), false, false);
		}
		//mkPgDir(dstart + 1, true, true, false, false, false, false, true, 0, 0x400000);
		//Map heap.
		for(i = 0; i < 1024; i++){
			mapPage(0x800000 + (i * 0x1000), 0xC0800000 + (i * 0x1000), true, false);
		}
		//mkPgDir(dstart + 2, true, true, true, false, false, false, true, 0, 0x800000);

		//Enable paging.
		lpagb(getPhysInt(pgDir));
		pagEnabled = true;
	}
}

//Get physical address.
void* getPhys(void* virt){
	unsigned int dirtab = (unsigned int)virt & 0xFFFFF000;
	if(pagEnabled){
		if(pgDir[GETDIR(dirtab)].present){
			struct pgTabEntry* tables = (struct pgTabEntry*)(0xFFC00000 + (GETDIR(dirtab) * 0x1000));
			struct pgTabEntry entry = tables[GETTAB(dirtab)];
			return (void*)((entry.page << 12) + ((unsigned int)virt & 0xFFF));
		}
	}
	return (void*)((unsigned int)virt - 0xC0000000);
}
