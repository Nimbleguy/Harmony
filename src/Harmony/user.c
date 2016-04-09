#include "user.h"

static unsigned int nrand[4] = {0xAAAAAAAA, 0xCAFE, 0x4, 0x101};

void makeMem(struct memHeader* head, struct memFooter* foot, bool avalible){
	head->magic = memSigH; //Header checksum.
	head->magic2 = memSig;
	head->address = head;
	head->foot = foot;
	head->check = ((unsigned int)head ^ (unsigned int)foot) >> 1;
	head->avalible = avalible;

	foot->magic = memSigF; //Footer checksum.
	foot->magic2 = memSig;
	foot->head = head;
	foot->check = head->check;
	foot->avalible = avalible;
}

void allocHeap(){
	//Allocate more memory to heap.
	mapPage((unsigned int)getPhys(HEAP_START) + heapSize, (unsigned int)HEAP_START + heapSize, true, true);
	struct memFooter* ofoot = (struct memFooter*)(HEAP_START + heapSize - sizeof(struct memFooter));
	struct memHeader* bhead = ofoot->head;
	struct memFooter* nfoot = (struct memFooter*)(HEAP_START + heapSize + HEAP_INCR - sizeof(struct memFooter));
	if(bhead->avalible){
		makeMem(bhead, nfoot, true);
		ofoot->magic = 0;
		ofoot->magic2 = 0;
	}
	else{
		struct memHeader* nhead = (struct memHeader*)(HEAP_START + heapSize);
		makeMem(nhead, nfoot, true);
	}
	heapSize += HEAP_INCR;
}

void combMem(){
	//Combine empty spaces.
	unsigned int i;
	unsigned int lAvalible = 0;
	unsigned int* mem = (unsigned int*)HEAP_START;
        struct memHeader* lastHead = 0;
	for(i = 0; i < (heapSize / 4); i++){
		if(mem[i] == memSigH && mem[i + 1] == memSig){
			struct memHeader* head = (struct memHeader*)(HEAP_START + (i * 4)); //Get header struct.
			if(head->check == ((unsigned int)head->address ^ (unsigned int)head->foot) >> 1){
				if(head->avalible){
					if(lAvalible){
						struct memHeader* lhead = (struct memHeader*)(HEAP_START + (lAvalible * 4)); //Get last header struct.
						head->foot->magic = 0; //Reset magic values.
						head->foot->magic2 = 0;
						lhead->magic = 0;
						lhead->magic2 = 0;
						makeMem(head, lhead->foot, true); //Configure header.
					}
					lAvalible = i;
				}
				else{
					lAvalible = 0;
				}
				lastHead = head;
			}
		}
	}
	//Check how far into the heap has been used, and allocate more space if bad things(tm)
	if(lastHead && (unsigned int)lastHead - (unsigned int)HEAP_START > heapSize - (HEAP_INCR / 8)){
		//Allocate more space.
		allocHeap();
	}
}

void* malloc(unsigned int s){
	combMem();
	unsigned int i;
	unsigned int* mem = (unsigned int*)HEAP_START;

	//Old junk.
	unsigned int size = s;

	//Allocate
	for(i = 0; i < (heapSize / 4); i++){
		if(mem[i] == memSigH && mem[i + 1] == memSig){
			struct memHeader* head = (struct memHeader*)(HEAP_START + (i * 4)); //Get header struct.
			if(head->check == ((unsigned int)head->address ^ (unsigned int)head->foot) >> 1){ //Checksum.
				if(head->avalible == true){ //Is this free?
					unsigned int storeSize = ((unsigned int)head->foot - (unsigned int)HEAP_START) - ((unsigned int)head->address - (unsigned int)HEAP_START) - sizeof(struct memHeader);
					//Check if good size and, if so, allocate memory.
					if(storeSize > size){
						if(storeSize - size - sizeof(struct memHeader) - sizeof(struct memFooter) < 0xFF){ //If it's not worth it to split.
							head->avalible = false;
							return (void*)((unsigned int)head->address + sizeof(struct memHeader));
						}
						else{
							struct memFooter* foot = head->foot;
							unsigned int newftmp = ((unsigned int)(head->address) + sizeof(struct memHeader) + size);
							unsigned int newhtmp = newftmp + sizeof(struct memFooter);
							struct memFooter* newf = (struct memFooter*)(newftmp + (4 - (newftmp % 4))); //Align
							struct memHeader* newh = (struct memHeader*)(newhtmp + (4 - (newhtmp % 4))); //Align
							makeMem(head, newf, false);
							makeMem(newh, foot, true);
							return (void*)((unsigned int)head->address + sizeof(struct memHeader));
						}
					}
					else if(storeSize == size){
						head->avalible = false;
						return (void*)((unsigned int)head->address + sizeof(struct memHeader));
					}
				}
			}
		}
	}
	//Not enough space.
	allocHeap();
	return malloc(size);
}

void free(void* addr){
	struct memHeader* head = (struct memHeader*)((unsigned int)addr - sizeof(struct memHeader));
	makeMem(head->address, head->foot, true);
	combMem();
}

void setupUsr(){
	//Randomize RNG.
	unsigned int random = rand();
	for(; !((random % 20) == 10); random = rand());

	//Memory management setup.
	heapSize = HEAP_INCR;
	memSigH = rand();
	memSigF = rand();
	memSig = rand();
	makeMem((struct memHeader*)HEAP_START, (struct memFooter*)(HEAP_START + heapSize - sizeof(struct memFooter)), true);
	ltssb();
	enablePaging(); //Redo tables now that malloc works.
}

void* memcpy(void* out, void* in, unsigned int bytes){
	unsigned int i;
	char* b1 = (char*)out;
	char* b2 = (char*)in;

	for(i = 0; i < bytes; i++){
		b1[i] = b2[i];
	}

	return b1;
}

void* memset(void* str, int c, unsigned int bytes){
	unsigned int i;
	unsigned char ch = (unsigned char)c;
	char* s = (char*)str;

	for(i = 0; i < bytes; i++){
		s[i] = ch;
	}

	return s;
}

void* memmove(void* out, void* in, unsigned int bytes){
	char* b1 = (char*)out;
	char* bi = malloc(bytes);
	char* b2 = (char*)in;

	memcpy(bi, b1, bytes);
	memcpy(b2, bi, bytes);

	free(bi);
	return b1;
}



int rand(){
	unsigned int n = nrand[0] + nrand[1] + nrand[2] + nrand[3];
	srand(n);
	return n;
}

void srand(unsigned int seed){
	nrand[0] = nrand[1];
	nrand[1] = nrand[2];
	nrand[2] = nrand[3];
	nrand[3] = seed;
}
