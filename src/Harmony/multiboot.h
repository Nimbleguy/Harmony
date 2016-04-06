#ifndef MULTIBOOTH
#define MULTIBOOTH

struct mbBoot{
	unsigned char biosDisk;
	unsigned char part1;
	unsigned char part2;
	unsigned char part3;
}__attribute__((packed));

struct mbInfo{
	unsigned int flags;
	unsigned int memLow;
	unsigned int memHigh;
	struct mbBoot boot;
	unsigned int cmdPhys;
	unsigned int modCount;
	unsigned int modAddr;
	unsigned int sym : 12;
	unsigned int mmapLength;
	unsigned int  mmapAddr;
	//Add more if needed.
}__attribute__((packed));

struct mbInfo* multiboot;

#endif
