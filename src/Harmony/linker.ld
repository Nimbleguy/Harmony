/* ELF */
/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(highloader)
OUTPUT_FORMAT(elf32-i386)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	/* Higher half kernel start. */
	. = 0xC0100000;
	
	.mbheader : AT(ADDR(.mbheader) - 0xC0000000){
		*(.mbheader)
	}
	
	/* First put the multiboot header, as it is required to be put very early
	   early in the image or the bootloader won't recognize the file format.
	   Next we'll put the .text section. */
	.text ALIGN(4K) : AT(ADDR(.text) - 0xC0000000)
	{
		*(.text)
	}

	/* Read-only data. */
	.rodata ALIGN(4K) : AT(ADDR(.rodata) - 0xC0000000)
	{
		*(.rodata*)
	}

	/* Read-write data (initialized) */
	.data ALIGN(4K) : AT(ADDR(.data) - 0xC0000000)
	{
		*(.data)
	}

	/* Read-write data (uninitialized) and stack */
	.bss ALIGN(4K) : AT(ADDR(.bss) - 0xC0000000)
	{
		*(COMMON)
		*(.bss)
	}

	/* End of file data, mostly the heap. */
	.eofile ALIGN(4K) : AT(ADDR(.eofile) - 0xC0000000)
	{
		*(.eofile)
	}
	
	/* The compiler may produce other sections, by default it will put them in
	   a segment with the same name. Simply add stuff here as needed. */
}
