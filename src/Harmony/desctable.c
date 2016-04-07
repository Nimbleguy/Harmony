#include "desctable.h"

void idtDesc(void* ptrr, unsigned short index);

void gdtDesc(unsigned int num, unsigned int base, unsigned int limit, unsigned char access, unsigned char gran){
	gdt[num].base_low = (base & 0xFFFF);
	gdt[num].base_middle = (base >> 16) & 0xFF;
	gdt[num].base_high = (base >> 24) & 0xFF;
	gdt[num].limit_low = (limit & 0xFFFF);
	gdt[num].granularity = ((limit >> 16) & 0x0F);
	gdt[num].granularity |= (gran & 0xF0);
	gdt[num].access = access;
}

void makeGDT(){
	gdtpos.size = (sizeof(struct gdtEntry) * 7) - 1;
	gdtpos.ptr = (int) &gdt;

	//Null Descriptor
	gdtDesc(0, 0, 0, 0, 0);
	//Code
	gdtDesc(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
	//Data
	gdtDesc(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
	//User Code
        gdtDesc(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
        //User Data
        gdtDesc(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);
	//User Stack Placeholder
	gdtDesc(5, 0, 0xFFFFFFFF, 0xF2, 0xCF);

	//Task State Segment
	memset(&tss0, 0, sizeof(struct tss));
	tss0.ss0 = 0x10;
	tss0.esp0 = 0;
	gdtDesc(6, (unsigned int)(&tss0), sizeof(struct tss), 0xE9, 0x0);

	//Activate Changes!
	lgdtb();
}

void setTssStack(void* ptr){
	tss0.esp0 = (unsigned int)ptr;
}

void pic(){
	//Initialize PICs
	//ICW1 - Various initialization
	outb(PIC1_C, 0x11);
	outb(PIC2_C, 0x11);
	//ICW2 - IDT offsetting
	//Will use IRQs 32 to 47
	outb(PIC1_D, 0x20);
	outb(PIC2_D, 0x28);
	//ICW3 - Cascade settings
	outb(PIC1_D, 0x4);
	outb(PIC2_D, 0x2);
	//ICW4 - Random generic settings
	outb(PIC1_D, 1);
	outb(PIC2_D, 1);
	//Null data registers.
	outb(PIC1_D, 0);
	outb(PIC2_D, 0);

	//Disable all interrupts except keyboard for now.
	outb(PIC1_D, inb(PIC1_D) & 0xFD);
	outb(PIC2_D, inb(PIC2_D) & 0xFF);
}

void makeIDT(){
	//Setup the PIC
	pic();
	//Set up the vectors
	unsigned short index = 0;
	idtDesc(interrupt0, index);
	index += 2;
	idtDesc(interrupt1, index);
        index += 2;
        idtDesc(interrupt2, index);
        index += 2;
        idtDesc(interrupt3, index);
        index += 2;
        idtDesc(interrupt4, index);
        index += 2;
        idtDesc(interrupt5, index);
        index += 2;
        idtDesc(interrupt6, index);
        index += 2;
        idtDesc(interrupt7, index);
        index += 2;
        idtDesc(interrupt8, index);
        index += 2;
	idtDesc(interrupt9, index);
	index += 2;
        idtDesc(interrupt10, index);
        index += 2;
        idtDesc(interrupt11, index);
        index += 2;
        idtDesc(interrupt12, index);
        index += 2;
        idtDesc(interrupt13, index);
        index += 2;
        idtDesc(interrupt14, index);
        index += 4; // Skips 1
        idtDesc(interrupt16, index);
        index += 2;
        idtDesc(interrupt17, index);
        index += 2;
        idtDesc(interrupt18, index);
        index += 2;
        idtDesc(interrupt19, index);
        index += 2;
        idtDesc(interrupt20, index);
        index += 20; //Skips 9
        idtDesc(interrupt30, index);
	index += 4; //Skips 1
	idtDesc(interrupt32, index);
	index += 2;
	idtDesc(interrupt33, index);
	index += 2;
	idtDesc(interrupt34, index);
        index += 2;
        idtDesc(interrupt35, index);
        index += 2;
        idtDesc(interrupt36, index);
        index += 2;
        idtDesc(interrupt37, index);
        index += 2;
        idtDesc(interrupt38, index);
        index += 2;
        idtDesc(interrupt39, index);
        index += 2;
        idtDesc(interrupt40, index);
        index += 2;
        idtDesc(interrupt41, index);
        index += 2;
        idtDesc(interrupt42, index);
        index += 2;
        idtDesc(interrupt43, index);
        index += 2;
        idtDesc(interrupt44, index);
        index += 2;
        idtDesc(interrupt45, index);
        index += 2;
        idtDesc(interrupt46, index);
        index += 2;
        idtDesc(interrupt47, index);
	// Load IDT!
	idtpos.size = (sizeof(struct idtEntry) * 256) -1;
	idtpos.ptr = (unsigned int) &idt;
	lidtb((unsigned int) &idtpos);
}

void idtDesc(void* ptrr, unsigned short indexx){
	unsigned short index = indexx / 2; //Because lazyness.
	unsigned int ptr = (unsigned int) ptrr;
	idt[index].ptrl = ptr & 0xFFFF;
	idt[index].ptrh = (ptr >> 16) & 0xFFFF;
	idt[index].segsel = 0x08;
	idt[index].zero = 0;
	idt[index].flags = 0x8E;
}

void interruptHandle(struct intState s){
	unsigned int i = s.i;
	if(i == 9){
		// NINE IS A LIE
		return;
	}

	//PIC interrupts
	if(i >= 32 && i <= 47){
		switch(i){
			case 32 + 1: ;
				//Keyboard
				char str[2] = "\0";
				str[0] = stc((unsigned char) inb(KEYBOARD_D));
				fbWrite(str, GREEN, BLACK);
				break;
			default:
				break;
		}
		if(i <= 39){
			outb(PIC1_C, PIC_EOI);
		}
		else if(i >= 40){
			outb(PIC1_C, PIC_EOI);
			outb(PIC2_C, PIC_EOI);
		}
		return;
	}

	fbWrite("INTERRUPT\n", WHITE, BLUE);
	switch(i){
		case 0:
			fbWrite("DIVIDE BY ZERO", WHITE, BLUE);
			break;
		case 1:
			fbWrite("DEBUG", WHITE, BLUE);
                        break;
                case 2:
			if(inb(0x61) == 6 || inb(0x61) == 7){
				fbWrite("                 PEBKAC ERROR\n", WHITE, BLUE);
				fbWrite("                 WHATEVER YOU DID, YOU SHOULD NOT HAVE DONE.\n", WHITE, BLUE);
				fbWrite("                 PLEASE REPLACE COMPUTER\n", WHITE, BLUE);
			}
			else{
				fbWrite("NONMASK INT ", WHITE, BLUE);
                        }
			fbWrite(hts(inb(0x92)), WHITE, BLUE);
			fbWrite(" ", WHITE, BLUE);
			fbWrite(hts(inb(0x61)), WHITE, BLUE);
			break;
                case 3:
                        fbWrite("BREAKPOINT", WHITE, BLUE);
                        break;
                case 4:
                        fbWrite("OVERFLOW", WHITE, BLUE);
                        break;
                case 5:
                        fbWrite("RANGE EXCEEDED", WHITE, BLUE);
                        break;
                case 6:
                        fbWrite("INVALID OPCODE", WHITE, BLUE);
                        break;
                case 7:
                        fbWrite("DEVICE NOT AVALIBLE", WHITE, BLUE);
                        break;
                case 8:
                        fbWrite("DOUBLE FAULT", WHITE, BLUE);
                        break;
		case 10:
			fbWrite("INVALID TSS", WHITE, BLUE);
                        break;
                case 11:
                        fbWrite("SEGMENT ABSENT", WHITE, BLUE);
                        break;
                case 12:
                        fbWrite("STACK-SEG FAULT", WHITE, BLUE);
                        break;
                case 13:
                        fbWrite("GEN PROT FAULT\n", WHITE, BLUE);
			if(s.errorcode & 0x00000001){
				fbWrite("EXT ", WHITE, BLUE);
			}
			switch((s.errorcode & 0x00000006) >> 1){
				case 0:
					fbWrite("GDT ", WHITE, BLUE);
					break;
				case 1:
					fbWrite("IDT ", WHITE, BLUE);
                                        break;
				case 2:
					fbWrite("LDT ", WHITE, BLUE);
                                        break;
				case 3:
					fbWrite("IDT ", WHITE, BLUE);
                                        break;
				default:
					fbWrite(hts((s.errorcode & 0x00000006) >> 1), WHITE, BLUE);
					fbWrite(" ", WHITE, BLUE);
					break;
			}
			fbWrite(hts((s.errorcode & 0x0000FFF8) >> 3), WHITE, BLUE);
                        break;
                case 14:
                        fbWrite("PAGE FAULT\n", WHITE, BLUE);
			if(s.errorcode & 0x1){
				fbWrite("PROT ", WHITE, BLUE);
			}
			else{
				fbWrite("404 ", WHITE, BLUE);
			}
			if(s.errorcode & 0x2){
				fbWrite("WRITE ", WHITE, BLUE);
			}
			else{
				fbWrite("READ ", WHITE, BLUE);
			}
			if(s.errorcode & 0x4){
				fbWrite("USR ", WHITE, BLUE);
			}
			else{
				fbWrite("KRN ", WHITE, BLUE);
			}
			if(s.errorcode & 0x8){
				fbWrite("RES OVRWRT ", WHITE, BLUE);
			}
			if(s.errorcode & 0x10){
				fbWrite("INSTR FETCH", WHITE, BLUE);
			}
			fbWrite("\n", WHITE, BLUE);
			fbWrite(hts(s.cr2), WHITE, BLUE);
                        break;
                case 16:
                        fbWrite("x87 FLOAT PT", WHITE, BLUE);
                        break;
                case 17:
                        fbWrite("ALIGN CHECK", WHITE, BLUE);
                        break;
                case 18:
                        fbWrite("MACHINE CHECK", WHITE, BLUE);
                        break;
                case 19:
                        fbWrite("SIMD FLOAT PT", WHITE, BLUE);
                        break;
                case 20:
                        fbWrite("VIRTUALIZATION", WHITE, BLUE);
                        break;
                case 30:
                        fbWrite("SECURITY", WHITE, BLUE);
                        break;
		default:
			fbWrite(hts(i), WHITE, BLUE);
			break;
	}
	fbWrite("\tEAX: ", WHITE, BLUE);
	fbWrite(hts(s.eax), WHITE, BLUE);
	fbWrite("\tEBX: ", WHITE, BLUE);
        fbWrite(hts(s.ebx), WHITE, BLUE);
	fbWrite("\tECX: ", WHITE, BLUE);
        fbWrite(hts(s.ecx), WHITE, BLUE);
	fbWrite("\tEDX: ", WHITE, BLUE);
        fbWrite(hts(s.edx), WHITE, BLUE);
	fbWrite("\tESI: ", WHITE, BLUE);
        fbWrite(hts(s.esi), WHITE, BLUE);
	fbWrite("\tEDI: ", WHITE, BLUE);
        fbWrite(hts(s.edi), WHITE, BLUE);
	fbWrite("\tEBP: ", WHITE, BLUE);
        fbWrite(hts(s.ebp), WHITE, BLUE);
	fbWrite("\tESP: ", WHITE, BLUE);
        fbWrite(hts(s.esp), WHITE, BLUE);

	fbWrite("\tERR: ", WHITE, BLUE);
	fbWrite(hts(s.errorcode), WHITE, BLUE);
        fbWrite("\tEIP: ", WHITE, BLUE);
        fbWrite(hts(s.eip), WHITE, BLUE);
        fbWrite("\tCS: ", WHITE, BLUE);
        fbWrite(hts(s.cs), WHITE, BLUE);
        fbWrite("\tEFLAGS: ", WHITE, BLUE);
        fbWrite(hts(s.eflags), WHITE, BLUE);

	loopf();
}
