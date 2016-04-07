#include "disk.h"

unsigned short ATA_IO = 0; //Selected ATA IO port.
unsigned short ATA_CR = 0; //Selected ATA Control Register.
unsigned short SELOFF = 0; //Selected ATA Drive Offset (0 for Master, 0x10 for Slave);

//Returns true if not errored.
bool hdWait(){
	unsigned char t = 0;
	while(true){
		unsigned char status = inb(ATA_IO + IO_STATUS);
		if((status & 0x1 || status & 0x20) && t >= 4){
			//Error or Drive Fault.
			return false;
		}
		if(!(status & 0x80)){
			//Not busy.
			if(status & 0x8){
				//Ready
				return true;
			}
		}
		if(t < 4){
			t++;
		}
	}
}

void nsDelay(){
	inb(ATA_IO + IO_STATUS);
	inb(ATA_IO + IO_STATUS);
	inb(ATA_IO + IO_STATUS);
	inb(ATA_IO + IO_STATUS);
}

bool setupHD(){
	partStart = 0;
	partEnd = 0xFFFFFFFF;
	bool secondary = true;
	//Detect first working drive.
	if(inb(ATA1_IO + IO_STATUS) != 0xFF){
		secondary = false;
		//Bus has a working drive. Run through the master and slave, identifying them for working order.
		outb(ATA1_IO + IO_DRIVE, 0xA0); //Select master.
		inb(ATA1_IO + IO_STATUS); //400ns delay.
		inb(ATA1_IO + IO_STATUS); //400ns delay.
		inb(ATA1_IO + IO_STATUS); //400ns delay.
		inb(ATA1_IO + IO_STATUS); //400ns delay.
		outb(ATA1_IO + IO_SECCNT, 0x0); //Reset some ports.
		outb(ATA1_IO + IO_SECNUM, 0x0);
		outb(ATA1_IO + IO_CYLL, 0x0);
		outb(ATA1_IO + IO_CYLH, 0x0);
		outb(ATA1_IO + IO_CMD, 0xEC); //Send identify command.
		if(inb(ATA1_IO + IO_STATUS) != 0){
			//The drive exists.
			ATA_IO = ATA1_IO;
			ATA_CR = ATA1_CR;
			SELOFF = 0x0;
			fbWrite("P - M\n", DTCOLOR, BLACK);
		}
		else{
			//Nonexistant master. Test slave.
			outb(ATA1_IO + IO_DRIVE, 0xB0); //Select slave.
			inb(ATA1_IO + IO_STATUS); //400ns delay.
			inb(ATA1_IO + IO_STATUS); //400ns delay.
			inb(ATA1_IO + IO_STATUS); //400ns delay.
			inb(ATA1_IO + IO_STATUS); //400ns delay.
			outb(ATA1_IO + IO_SECCNT, 0x0); //Reset some ports.
			outb(ATA1_IO + IO_SECNUM, 0x0);
			outb(ATA1_IO + IO_CYLL, 0x0);
			outb(ATA1_IO + IO_CYLH, 0x0);
			outb(ATA1_IO + IO_CMD, 0xEC); //Send identify.
			if(inb(ATA1_IO + IO_STATUS) != 0){
				//It exists.
				ATA_IO = ATA1_IO;
				ATA_CR = ATA1_CR;
				SELOFF = 0x10;
				fbWrite("P - S\n", DTCOLOR, BLACK);
			}
			else{
				//IDK
				secondary = true;
			}
		}
	}
	bool nodrive = secondary;
	if(inb(ATA2_IO + IO_STATUS) != 0xFF && secondary){
		nodrive = false;
		//Bus has a working drive. Yada yada yada.
		outb(ATA2_IO + IO_DRIVE, 0xA0);
		inb(ATA1_IO + IO_STATUS); //400ns delay.
		inb(ATA1_IO + IO_STATUS); //400ns delay.
		inb(ATA1_IO + IO_STATUS); //400ns delay.
		inb(ATA1_IO + IO_STATUS); //400ns delay.
		outb(ATA2_IO + IO_SECCNT, 0x0);
		outb(ATA2_IO + IO_SECNUM, 0x0);
		outb(ATA2_IO + IO_CYLL, 0x0);
		outb(ATA2_IO + IO_CYLH, 0x0);
		outb(ATA2_IO + IO_CMD, 0xEC);
		if(inb(ATA2_IO + IO_STATUS) != 0){
			ATA_IO = ATA2_IO;
			ATA_CR = ATA2_CR;
			SELOFF = 0x0;
			fbWrite("S - M\n", DTCOLOR, BLACK);
		}
		else{
			outb(ATA2_IO + IO_DRIVE, 0xB0);
			inb(ATA1_IO + IO_STATUS); //400ns delay.
			inb(ATA1_IO + IO_STATUS); //400ns delay.
			inb(ATA1_IO + IO_STATUS); //400ns delay.
			inb(ATA1_IO + IO_STATUS); //400ns delay.
			outb(ATA2_IO + IO_SECCNT, 0x0);
			outb(ATA2_IO + IO_SECNUM, 0x0);
			outb(ATA2_IO + IO_CYLL, 0x0);
			outb(ATA2_IO + IO_CYLH, 0x0);
			outb(ATA2_IO + IO_CMD, 0xEC);
			if(inb(ATA2_IO + IO_STATUS) != 0){
				ATA_IO = ATA2_IO;
				ATA_CR = ATA2_CR;
				SELOFF = 0x10;
				fbWrite("S - S\n", DTCOLOR, BLACK);
			}
			else{
				nodrive = true;
			}
		}
	}
	//No drives
	if(nodrive){
		fbWrite("No hard drives detected!\n", DTCOLOR, BLACK);
		return false;
	}

	outb(ATA_CR, 0x1); //Set control register to disable interrupts. I DON'T CARE.

	return true;
}

void hdWrite(void* in, unsigned int loc, unsigned int bytes){
        unsigned int lma = (loc + partStart) & 0x0FFFFFFF;
        if(lma > partEnd || lma < partStart){
                fbWrite("Attempted to set out of bounds LMA!\n", RED, BLACK);
                return;
        }
        hdWriteAbs(in, lma, bytes);
}

void hdWriteAbs(void* in, unsigned int loc, unsigned int bytes){
	unsigned int sectors = (bytes + 512 - 1) / 512; //Round up: (A + B - 1)/B
	unsigned int lma = loc & 0x0FFFFFFF;

	outb(ATA_IO, (0xE0 + SELOFF) | (lma >> 24)); //Send 0xE0 for master, 0xF0 for slave. OR with highest 4 bits of LGA.
	outb(ATA_IO + IO_SECCNT, sectors); //Output amount of sectors to read.
	outb(ATA_IO + IO_LGAl, lma & 0xFF); //Output lower 8 bits of LMA.
	outb(ATA_IO + IO_LGAm, (lma >> 8) & 0xFF); //Output middle 8 bits of LMA.
	outb(ATA_IO + IO_LGAh, (lma >> 16) & 0xFF); //Output high 8 bits of LMA.

	unsigned short* ins = (unsigned short*)in; //Make it point to 16 bit thingies.

	outb(ATA_IO + IO_CMD, 0x30); //Send write sectors command.
	unsigned short i;
	unsigned short b;
	for(i = 0; i < sectors; i++){
		hdWait();
		for(b = i * 256; b < (256 + (i * 256)); b++){
			//I = sector, B = 2-byte-big data thingies.
			if(b < bytes){
				//If less than max.
				outb(ATA_IO + IO_DATA, ins[b]); //Input data.
			}
		}
		nsDelay();
	}
	outb(ATA_IO + IO_CMD, 0xE7); //Cache Flush.
	hdWait();
}

void hdRead(void* out, unsigned int loc, unsigned int bytes){
	unsigned int lma = (loc + partStart) & 0x0FFFFFFF;
	if(lma > partEnd || lma < partStart){
		fbWrite("Attempted to access out of bounds LMA!\n", RED, BLACK);
                return;
        }
	hdReadAbs(out, lma, bytes);
}

void hdReadAbs(void* out, unsigned int loc, unsigned int bytes){
	unsigned int sectors = (bytes + 512 - 1) / 512; //Round up: (A + B - 1)/B
	unsigned int lma = loc & 0x0FFFFFFF;

	outb(ATA_IO, (0xE0 + SELOFF) | (lma >> 24)); //Send 0xE0 for master, 0xF0 for slave. OR with highest 4 bits of LGA.
	outb(ATA_IO + IO_SECCNT, sectors); //Output amount of sectors to read.
	outb(ATA_IO + IO_LGAl, lma & 0xFF); //Output lower 8 bits of LMA.
	outb(ATA_IO + IO_LGAm, (lma >> 8) & 0xFF); //Output middle 8 bits of LMA.
	outb(ATA_IO + IO_LGAh, (lma >> 16) & 0xFF); //Output high 8 bits of LMA.

	unsigned short* outs = (unsigned short*)out;

	outb(ATA_IO + IO_CMD, 0x20); //Send read sectors command.
	unsigned short i;
	unsigned short b;
	for(i = 0; i < sectors; i++){
		hdWait();
		for(b = i * 256; b < (256 + (i * 256)); b++){
			//I = sector, B = 2-byte-big data thingies.
			if(b < bytes){
				//If less than max.
				outs[b] = inb(ATA_IO + IO_DATA); //Input data.
			}
		}
		nsDelay();
	}
}