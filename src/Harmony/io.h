#ifndef IOH
#define IOH

#include "common.h"
#include "desctable.h"

unsigned char inb(unsigned short port);
void outb(unsigned short port, unsigned char data);
void lgdtb();
void lidtb(unsigned int i);
void loopf();
void lpagb(unsigned int i);
void lvgab(unsigned int* i); //Needs to be 4KB big.
void lusrb(unsigned int i, unsigned int* sp);
void invlpgb();
struct genReg callb(unsigned int addr);
void ltssb();
void clib();
void stib();

#endif
