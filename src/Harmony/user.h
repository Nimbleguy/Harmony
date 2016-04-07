#ifndef USERH
#define USERH

#include "common.h"
#include "desctable.h"
#include "utilities.h"
#include "paging.h"
#include "terminal.h"

#define HEAP_START (void*)0xC0400000
#define HEAP_INCR 0x400000

struct memHeader{
	unsigned int magic;
	unsigned int magic2;
	unsigned int check : 31;
	unsigned int avalible : 1;
	struct memHeader* address;
	struct memFooter* foot;
}__attribute__((packed));

struct memFooter{
	struct memHeader* head;
	unsigned int check : 31;
        unsigned int avalible : 1;
	unsigned int magic2;
	unsigned int magic;
}__attribute__((packed));

unsigned int heapSize;
unsigned int memSigH;
unsigned int memSigF;
unsigned int memSig;

void setupUsr();
void* malloc(unsigned int size);
void free(void* addr);
void* memcpy(void* out, void* in, unsigned int bytes);
void* memset(void* str, int c, unsigned int bytes);
void* memmove(void* out, void* in, unsigned int bytes);
int rand();
void srand(unsigned int seed);

#endif
