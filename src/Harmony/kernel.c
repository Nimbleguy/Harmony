#include "kernel.h"

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

void kernel_main(void* heap) {
	fbWrite("Bootloading complete\n", DTCOLOR, BLACK);

	//Do the heap.
	frHeap = (unsigned int)heap;
	fbWrite(its(frHeap), DTCOLOR, BLACK);

	fbWrite("\nLoading GDT...\n", DTCOLOR, BLACK);
	//Initialize the VERY TRUELY EXTREMELY IMPORTANT GDT!
	makeGDT();
	fbWrite("GDT Loaded.\n", DTCOLOR, BLACK);

	fbWrite("Loading IDT...\n", DTCOLOR, BLACK);
	//We need to be able to crash.
	makeIDT();
	fbWrite("IDT Loaded.\n", DTCOLOR, BLACK);

	fbWrite("Loading Paging...\n", DTCOLOR, BLACK);
	//Now for paging, which is GDT 2: Electric Boogaloo.
	enablePaging();
	fbWrite("Paging Loaded.\n", DTCOLOR, BLACK);

	fbWrite("Loading Libraries of the Third Ring...\n", DTCOLOR, BLACK);
	//User junk.
	setupUsr();
	fbWrite("\nThird Libraries Loaded.\n", DTCOLOR, BLACK);

	fbWrite("Loading Hard Drive...\n", DTCOLOR, BLACK);
        //HDD
        setupHD();
        fbWrite("Hard Drive Loaded.\n", DTCOLOR, BLACK);

	fbWrite("Loading Filesystem...\n", DTCOLOR, BLACK);
        //FS
        setupFS();
        fbWrite("Filesystem Loaded.\n", DTCOLOR, BLACK);

	// Make sure screan is clear from junk.
	fbClear(BLACK);

	// Print stuff.
	fbWrite("| Harmony Alpha |\n\n", DTCOLOR, BLACK);

	// User Mode
//	lusrb(0);
}
