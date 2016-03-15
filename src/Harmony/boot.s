; Entry point
global highloader

section .mbheader
align 4
	; Multiboot Magic
	MAGIC equ 0x1BADB002 ; One bad boot?
	FLAGS equ 0x0
	CHECKSUM equ -(MAGIC + FLAGS)
	dd MAGIC
	dd FLAGS
	dd CHECKSUM



section .data

; Kernel virtual memory base & page directory index
KERN_VIRB equ 0xC0000000
KERN_PG_NUM equ (KERN_VIRB >> 22)

align 0x1000

; Set up paging directory for the first 4MB, because higher half.
tempPgDir:
	dd 0x00000083
	times (KERN_PG_NUM - 1) dd 0 ; Pages before kernel space.
	dd 0x00000083
	times (1024 - KERN_PG_NUM - 1) dd 0 ; Pages after kernel.

; Stack size in bytes.
STACKSIZE equ 16384



; Start of Basically Stuff and Stuff section
section .bss
align 32
kstack:
	; Reserve STACKSIZE bytes.
	resb STACKSIZE



; Start of code section
section .text

; Entry point.
highloader equ (hloader - KERN_VIRB)


; Enable paging for the first 4 MB, to be replaced later.
hloader:
	; Load page dir
	mov eax, (tempPgDir - KERN_VIRB)
	mov cr3, eax
	
	; Enable 4MB pages
	mov eax, cr4
	or eax, 0x00000010
	mov cr4, eax
	; Enable paging
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
	
	; Jump to true loader.
	lea eax, [loader]
	jmp eax

; Define entry point.
loader:
	; Disable identity mapping, no longer needed.
	mov dword [tempPgDir], 0
	invlpg [0]
	
	; Set stack pointer
	mov esp, kstack + STACKSIZE
	; Set heap info for kernel.
	lea eax, [kheap]

	cli
	extern kernel_main

	add ebx, KERN_VIRB
	push ebx
	push eax
	call kernel_main
	add esp, 8

; Loop forever when done.
.loop:
	hlt
	jmp .loop

; End of file, for heap reasons.
section .eofile

kheap:
	hlt ; To prevent optimizing?
