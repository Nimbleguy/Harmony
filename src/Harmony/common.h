#ifndef COMMONH
#define COMMONH

//ONLY DEFINE IF DOG
//#define DOG

#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

typedef unsigned char bool;
#define true 1
#define false 0
#define TRUE 1
#define FALSE 0

typedef unsigned int (*ptrmethod)(unsigned int);

#define KERN_START 0x100000

#endif
