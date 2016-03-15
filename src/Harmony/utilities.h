#ifndef UTILITIESH
#define UTILITIESH

#include "common.h"
#include "desctable.h"
#include "io.h"
#include "terminal.h"
#include "user.h"

#define kalloc(size) genkalloc(size, false, 0)
#define kalloc_al(size) genkalloc(size, true, 0) //Align
#define kalloc_ph(size, phys) genkalloc(size, false, phys) //With a physical address.
#define kalloc_alph(size, phys) genkalloc(size, true, phys) //Aligned & with a physical address.

char* its(unsigned int i);
char stc(unsigned char c);
void* genkalloc(unsigned int size, bool align, unsigned int* phys);

bool keyShift;
unsigned int frHeap;

#endif
