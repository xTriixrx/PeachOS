; Need to specify assembly origin so that the assembler knows how to offset our data
; The BIOS loads the bootloader at address 0x7c00
ORG 0

; Next we need to tell the assembler we are using 16 bit architecture (real mode)
; This will ensure the assembler will only assemble instructions into 16 bit code
BITS 16

_start:
    jmp short start
    nop

times 33 db 0 ; BIOS Parameter Block in case BIOS writes

start:
    jmp 0x7c0:init ; Will set CS register to 0x7c0

; Defining a divide by zero interrupt handler (0)
handle_zero:
    mov ah, 0eh
    mov al, 'A'
    mov bx, 0x00
    int 0x10
    iret

; Define debug interrupt handler (1)
handle_one:
    mov ah, 0eh
    mov al, 'V'
    mov bx, 0x00
    int 0x10
    iret

init:
    cli ; Clear interrupts flag
    mov ax, 0x7c0
    mov ds, ax ; Set up DS register to origin
    mov es, ax ; Set up ES register to origin
    mov ax, 0x00
    mov ss, ax ; Set up SS register to 0x00
    mov sp, 0x7c00 ; Set up SP register to origin

    sti ; Enables interrupts

    ; ss specifies stack segment; otherwise would use data segment by default
    ; The interrupt handler to be executed is dictated by the absolute address calculatation of the OFFSET:SEGMENT
    ; By inserting the address of the handler function as the offset and the appropriate segment into the interrupts segment
    ; On an interrupt we can have our own interrupt's handled.

    ; Inserts handle_zero address into the offset for interrupt zero position
    mov word[ss:0x00], handle_zero
    ; Inserts the bootloader origin address as the interrupt zero segment
    mov word[ss:0x02], 0x7c0

    ; Trigger divide by zero exception
    mov ax, 0x00
    div ax

    ; Inserts handle_one address into the offset for interrupt one position
    mov word[ss:0x04], handle_one
    ; Inserts the bootloader origin address as the interrupt one segment
    mov word[ss:0x06], 0x7c0

    ; Triggers interrupt 1 directly
    int 1

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