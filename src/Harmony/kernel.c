#include "kernel.h"

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

void kernel_main(struct mbInfo* mb, void* heap) {
	fbWrite("Bootloading complete\n", DTCOLOR, BLACK);

	//Do the heap.
	frHeap = (unsigned int)heap;
	fbWrite(its(frHeap), DTCOLOR, BLACK);

	//Do the boot.
	multiboot = mb;

	fbWrite("\nLoading GDT...\n", DTCOLOR, BLACK);
	//Initialize the VERY TRUELY EXTREMELY IMPORTANT GDT!
	makeGDT();

	fbWrite("Loading IDT...\n", DTCOLOR, BLACK);
	//We need to be able to crash.
	makeIDT();

	fbWrite("Loading Paging...\n", DTCOLOR, BLACK);
	//Now for paging, which is GDT 2: Electric Boogaloo.
	enablePaging();

	fbWrite("Loading Libraries of the Third Ring...\n", DTCOLOR, BLACK);
	//User junk.
	setupUsr();

	fbWrite("Loading Hard Drive...\n", DTCOLOR, BLACK);
        //HDD
        if(!setupHD()){
		loopf();
	}

	fbWrite("Loading Filesystem...\n", DTCOLOR, BLACK);
        //FS
        if(!setupFS()){
		loopf();
	}

	// Make sure screan is clear from junk.
	fbClear(BLACK);

	// Print stuff.
	fbWrite("| Harmony Alpha |\n\n", DTCOLOR, BLACK);

	// User Mode
//	lusrb(0);
}
