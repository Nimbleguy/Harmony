extern interruptHandle

; Interrupt without error code.
%macro necInterrupt 1
global interrupt%1
align 4
interrupt%1:
	; Push error code 0, because no error code.
	push dword 0
	; Push interrupt number for C use.
	push dword %1
	jmp interruptc
%endmacro

; Interrupt with error code.
%macro ecInterrupt 1
global interrupt%1
align 4
interrupt%1:
	; Push interrupt number for C use.
	push dword %1
	jmp interruptc
%endmacro

; Common interrupt handler.
interruptc:

	; Save registers
	pushad
	mov cr2, eax
	push eax

	; Call the C function for this stuff.
	call interruptHandle

	; Load registers.
	pop eax
	popad

	; Add to stack pointer to compensate for push error code & stuff
	add esp, 12

	; Return to interrupted code.
	iret

; Interrupts. Remember to add to H file and desctable!

necInterrupt 0 ; Divide by 0
necInterrupt 1 ; Debug
necInterrupt 2 ; Nonmaskable
necInterrupt 3 ; Breakpoint
necInterrupt 4 ; Overflow
necInterrupt 5 ; Bound Range Exceeded
necInterrupt 6 ; Invalid Opcode
necInterrupt 7 ; Device Not Avalible

ecInterrupt 8 ; DOUBLE FAULT - VERY ERROR MUCH WOW

necInterrupt 9 ; Nine is a lie.

ecInterrupt 10 ; Invalid TSS
ecInterrupt 11 ; Segment not present
ecInterrupt 12 ; Stack-Segment
ecInterrupt 13 ; General. Protection. Fault. This is what killed the MkI.
ecInterrupt 14 ; Page Fault

necInterrupt 16 ; x87 Floating Point

ecInterrupt 17 ; Alignment Check

necInterrupt 18 ; Machine Check
necInterrupt 19 ; SIMD Floating-Point
necInterrupt 20 ; Virtualization

ecInterrupt 30 ; Security

necInterrupt 32 ; Timer
necInterrupt 33 ; Keyboard
necInterrupt 34 ; PIC 2
necInterrupt 35 ; COM 2
necInterrupt 36 ; COM 1
necInterrupt 37 ; LPT 2
necInterrupt 38 ; Floppy Disk
necInterrupt 39 ; LPT 1
necInterrupt 40 ; Real Time Clock
necInterrupt 41 ; Gen I/O
necInterrupt 42 ; Gen I/O
necInterrupt 43 ; Gen I/O
necInterrupt 44 ; Gen I/O
necInterrupt 45 ; Coprocessor Overrun
necInterrupt 46 ; IDE Bus
necInterrupt 47 ; IDE Bus
