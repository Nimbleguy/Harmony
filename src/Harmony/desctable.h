#ifndef DESCTABLEH
#define DESCTABLEH

#include "common.h"
#include "io.h"
#include "interrupt.h"
#include "utilities.h"
#include "terminal.h"

#define PIC1_C 0xC0000020u
#define PIC2_C 0xC00000A0u
#define PIC1_D 0xC0000021u
#define PIC2_D 0xC00000A1u
#define PIC_EOI 0x20
#define KEYBOARD_D 0xC0000060u
#define GDTSIZE 7

struct gdtPoint {
	unsigned short size;
	unsigned int ptr;
}__attribute__((packed));

struct gdtEntry {
	unsigned short limit_low;
	unsigned short base_low;
	unsigned char base_middle;
	unsigned char access;
	unsigned char granularity;
	unsigned char base_high;
}__attribute__((packed));

struct idtPoint{
	unsigned short size;
	unsigned int ptr;
}__attribute__((packed));

struct idtEntry{
	unsigned short ptrl;
	unsigned short segsel;
	unsigned char zero;
	unsigned char flags;
	unsigned short ptrh;
}__attribute__((packed));

struct intState {
	unsigned int cr2;
	unsigned int edi;
        unsigned int esi;
        unsigned int ebp;
        unsigned int esp;
        unsigned int ebx;
        unsigned int edx;
        unsigned int ecx;
        unsigned int eax;

	unsigned int i;
        unsigned int errorcode;
        unsigned int eip;
        unsigned int cs;
        unsigned int eflags;
	unsigned int useresp;
	unsigned int ss;
}__attribute__((packed));

struct tss{
	unsigned int ptss;
	unsigned int esp0;
        unsigned int ss0;
        unsigned int esp1;
        unsigned int ss1;
        unsigned int esp2;
        unsigned int ss2;
        unsigned int cr3;
        unsigned int eip;
        unsigned int eflags;
        unsigned int eax;
        unsigned int ecx;
        unsigned int edx;
        unsigned int ebx;
        unsigned int esp;
        unsigned int ebp;
        unsigned int esi;
        unsigned int edi;
        unsigned int es;
        unsigned int cs;
        unsigned int ss;
        unsigned int ds;
        unsigned int fs;
        unsigned int gs;
        unsigned int ldt;
        unsigned int trap;
        unsigned int iomap;
}__attribute__((packed));

struct gdtPoint gdtpos;
struct gdtEntry gdt[GDTSIZE];
struct idtPoint idtpos;
struct idtEntry idt[256];
struct tss tss0;

void makeGDT();
void makeIDT();
void interruptHandle(struct intState s);
void gdtDesc(unsigned int num, unsigned int base, unsigned int limit, unsigned char access, unsigned char gran);
void setTssStack(void* ptr);

#endif
