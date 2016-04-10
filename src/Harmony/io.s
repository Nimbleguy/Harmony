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
extern userStack

%macro startAsm 0
push ebp
mov ebp, esp
%endmacro
%macro endAsm 0
mov esp, ebp
pop ebp
ret
%endmacro

section .text

; Get data from IO from C
inb:
	startAsm
	mov dx, [ebp + 8]
	in ax, dx
	endAsm

; Send data to IO port from C
outb:
	startAsm
	; Move input into usable registers.
	mov ax, [ebp + 12]
	mov dx, [ebp + 8]
	; Output it.
	out dx, ax
	endAsm

; Load the GDT
lgdtb:
	startAsm
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
	endAsm

; Load the IDT
lidtb:
	startAsm
	cli
	mov eax, [esp + 8]
	lidt [eax]
	sti
	endAsm

; Loop forever, don't need to set up ebp becuase this won't exit.
loopf:
	hlt
	jmp loopf

; Load the page directory
lpagb:
	startAsm
	; Load new page directory
        mov eax, [esp + 8]
	mov cr3, eax
	invlpg [0]

	; Enable paging.
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
	endAsm

; Load VGA bitmap.
lvgab:
	startAsm
	mov esi, [esp + 8]
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
	endAsm

; Load luser mode. No ebp becuase no return.
lusrb:
	cli
	; Set data segment.
	mov ax, 0x23
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	mov ecx, [esp + 8]
	mov [ecx], esp
	mov ebx, [esp + 12] ; Setup instruction pointer for push.

	push 0x23 ; Set stack segment.
	push userStack ; Push stack pointer.
	pushf ; Push EFLAGS
	pop eax ; Enable interrupt flag.
	or eax, 0x200
	push eax
	push 0x1B ; Push code segment.
	push ebx ; Push instruction pointer.
	iret

invlpgb:
	startAsm
	invlpg [0]
	endAsm

ltssb:
	startAsm
	; Set TSS register to segment.
	mov ax, 0x2B
	ltr ax
	endAsm

clib:
	startAsm
	cli
	endAsm
stib:
	startAsm
	sti
	endAsm
