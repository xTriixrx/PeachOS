; DISK - READ SECTOR(S) INTO MEMORY

; AH = 02h
; AL = number of sectors to read (must be nonzero)
; CH = low eight bits of cylinder number
; CL = sector number 1-63 (bits 0-5)
; high two bits of cylinder (bits 6-7, hard disk only)
; DH = head number
; DL = drive number (bit 7 set for hard disk)
; ES:BX -> data buffer

; Return:
; CF set on error
; if AH = 11h (corrected ECC error), AL = burst length
; CF clear if successful
; AH = status (see #00234)
; AL = number of sectors transferred (only valid if CF set for some BIOSes)


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

init:
    cli ; Clear interrupts flag
    mov ax, 0x7c0
    mov ds, ax ; Set up DS register to origin
    mov es, ax ; Set up ES register to origin
    mov ax, 0x00
    mov ss, ax ; Set up SS register to 0x00
    mov sp, 0x7c00 ; Set up SP register to origin

    sti ; Enables interrupts

    mov ah, 0x2 ; READ SECTOR COMMAND
    mov al, 0x1 ; ONE SECTOR TO READ
    mov ch, 0x0 ; Cylinder low eight bits
    mov cl, 0x2 ; Read sector two
    mov dh, 0x0 ; Head number
    mov bx, buffer
    int 0x13

    jc error ; If carry is set, will jump to error label

    mov si, buffer
    call print

    ; Will keep jumping to itself to prevent executing signature
    jmp $

error:
    mov si, error_message
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

error_message: db 'Failed to load sector', 0

; Fill at least 510 bytes of data, whatever bytes are not used by the program are filled with zeros with this instruction
times 510-($ - $$) db 0

; Datawrite (assemble word) 0xAA55 (is flipped due to little-endianness)
dw 0xAA55

; Buffer label is placed at bottom of file to prevent it overwriting any code, will be written into
; Notice this label is outside of the first sector, this is intentional; you can reference labels outside of sector but
; you cannot reference labels with data.
buffer: