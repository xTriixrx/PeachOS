[BITS 32] ; All code below this will be seen as 32 bit code

global _start ; Exports _start symbol
global kernel_registers

extern kernel_main

CODE_SEG equ 0x08
DATA_SEG equ 0x10

_start:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov ebp, 0x00200000
    mov esp, ebp ; Moving stack pointer further in memory

    ; Enable A20 Line
    in al, 0x92 ; Read from processor bus port 0x92 into al
    or al, 2 ; 
    out 0x92, al ; Write AL to processor bus port 0x92

    ; Remap the master PIC
    mov al, 00010001b ; Puts PIC into initialization mode
    out 0x20, al ; Tell master PIC

    mov al, 0x20 ; Interrupt 0x20 is where master ISR should start
    out 0x21, al ; Tell master PIC

    ; Put master PIC in x86 mode
    mov al, 00000001b
    out 0x21, al
    ; End of remaster of master PIC

    call kernel_main

    jmp $

kernel_registers:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov fs, ax
    ret

; Fill at least 512 bytes of data, whatever bytes are not used by the program are filled with zeros with this instruction
times 512-($ - $$) db 0