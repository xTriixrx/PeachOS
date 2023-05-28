; Need to specify assembly origin so that the assembler knows how to offset our data
; The BIOS loads the bootloader at address 0x7c00
ORG 0x7C00

; Next we need to tell the assembler we are using 16 bit architecture (real mode)
; This will ensure the assembler will only assemble instructions into 16 bit code
BITS 16

start:
    ; Move address of label message into SI register
    mov si, message

    ; Call print subroutine
    call print

    ; Will keep jumping to itself to prevent executing signature
    jmp $

print:
    mov bx, 0 ; Part of oeh BIOS call to set page number (zero in this case)

; Subroutine is only accessible to parent subroutine print
.loop:
    lodsb ; Load char that SI register is pointing to into AL register and increment SI register to point to next char address
    cmp al, 0 ; Compare value in AL register to 0
    je .done ; If AL is 0, JE (jump equals) to .done subroutine
    call print_char ; Call print_char routine
    jmp .loop ; Jump back to start of .loop subroutine

; Subroutine label is only accessible to parent subroutine, print   
.done:
    ret
;
print_char:
    mov ah, 0eh
    int 0x10
    ret

; Label message for some databyte containing 'Hello World'
message: db 'Hello World!', 0

; Fill at least 510 bytes of data, whatever bytes are not used by the program are filled with zeros with this instruction
times 510-($ - $$) db 0

; Datawrite (assemble word) 0xAA55 (is flipped due to little-endianness)
dw 0xAA55