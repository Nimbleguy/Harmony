global inb
global outb
global lgdtb
global lidtb
global loopf
global lpagb
global lvgab
global lusrb
global invlpgb
global callb
global ltssb
global clib
global stib

extern gdtpos
extern idtpos

section .text

; Get data from IO from C
inb:
	mov dx, [esp + 4]
	in al, dx
	ret

; Send data to IO port from C
outb:
	; Move input into usable registers.
	mov al, [esp + 8]
	mov dx, [esp + 4]
	; Output it.
	out dx, al
	ret

; Load the GDT
lgdtb:
	lgdt [gdtpos]
	; Set Data Storage registers with GDT offset for data
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	; Set Code Storage register with GDT offset, and FAR JUMP TO IT.
	jmp 0x08:flushcs

; Far Jump!
flushcs:
	ret

; Load the IDT
lidtb:
	cli
	mov eax, [esp + 4]
	lidt [eax]
	sti
	ret

; Loop forever
loopf:
	hlt
	jmp loopf

; Load the page directory
lpagb:
	; Load new page directory
        mov eax, [esp + 4]
	mov cr3, eax
	invlpg [0]

	; Enable paging.
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
	ret

; Load VGA bitmap.
lvgab:
	mov esi, [esp + 4]
	; Clear even/odd mode.
	mov dx, 0x3CE
	mov ax, 5
	out dx, ax
	; Map VGA memory to 0xC000A0000
	mov ax, 0x406
	out dx, ax
	; Set bitplane 2
	mov dx, 0x3c4
	mov ax, 0x402
	out dx, ax
	; Clear even/odd mode another way?
	mov ax, 0x604
	out dx, ax
	; Set destination buffer and counter.
	mov edi, 0xC00A0000
	mov ecx, 256
.l:
	; Copy 16 bytes to the charmap.
	movsd
	movsd
	movsd
	movsd
	; Skip 16 bytes and loop 256 times, from ecx.
	add edi, 16
	loop .l
	; Restore normal VGA state.
	mov ax, 0x302
	out dx, ax
	mov ax, 0x204
	out dx, ax
	mov dx, 0x3CE
	mov ax, 0x1005
	out dx, ax
	mov ax, 0xE06
	out dx, ax
	ret

; Load luser mode.
lusrb:
	cli
	; Set data segment.
	mov ax, 0x23
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	mov ebx, [esp + 4] ; Setup instruction pointer for push.

	push 0x2B ; Set stack segment.
	push 0x100000 ; Push stack pointer, which is new to the stack segment.
	pushf ; Push EFLAGS
	pop eax ; Enable interrupt flag.
	or eax, 0x200
	push eax
	push 0x1B ; Push code segment.
	push ebx ; Push instruction pointer.
	iret
.test:
	cli
	hlt
	jmp .test

invlpgb:
	invlpg [0]
	ret

ltssb:
	; Set TSS register to segment.
	mov ax, 0x33
	ltr ax
	ret

clib:
	cli
	ret
stib:
	sti
	ret
