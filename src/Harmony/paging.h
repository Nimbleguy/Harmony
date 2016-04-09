#ifndef PAGINGH
#define PAGINGH

#include "common.h"
#include "utilities.h"
#include "io.h"

#define GETDIR(x) ((x) / 0x400000)
#define GETTAB(x) (((x) % 0x400000) / 0x1000)
#define getPhysInt(x) (unsigned int)getPhys((void*)(x))

struct pgDirEntry{
	bool present : 1;
	bool rw : 1;
	bool usr : 1;
	bool wcache : 1;
	bool dcache : 1;
	bool accessed : 1;
	bool zero : 1;
	bool size : 1;
	unsigned char free : 4;
	unsigned int table : 20;
}__attribute__((packed));

struct pgTabEntry{
	bool present : 1;
        bool rw : 1;
        bool usr : 1;
        bool wcache : 1;
        bool dcache : 1;
        bool accessed : 1;
	bool dirty : 1;
        bool zero : 1;
        bool global : 1;
        unsigned char free : 3;
        unsigned int page : 20;
}__attribute__((packed));

struct pgDirEntry* pgDir; //Temp page dir for switching.
struct pgDirEntry* pgDirT; //True page dir.

bool pagEnabled;

void enablePaging();
void* getPhys(void* virt);
void mapPage(unsigned int phys, unsigned int virt, bool user, bool size);
void switchPage();

#endif
