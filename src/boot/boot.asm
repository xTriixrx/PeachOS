; Need to specify assembly origin so that the assembler knows how to offset our data
; The BIOS loads the bootloader at address 0x7c00
ORG 0x7c00

; Next we need to tell the assembler we are using 16 bit architecture (real mode)
; This will ensure the assembler will only assemble instructions into 16 bit code
BITS 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

jmp short start
nop

; FAT16 Header
OEMIdentifier       db 'PEACHOS '
BytesPerSector      dw 0x200
SectorsPerCluster   db 0x80
ReservedSectors     dw 200
FATCopies           db 0x02
RootDirEntries      dw 0x40
NumSectors          dw 0x00
MediaType           db 0xF8
SectorsPerFat       dw 0x100
SectorsPerTrack     dw 0x20
NumberOfHeads       dw 0x40
HiddenSectors       dd 0x00
SectorsBig          dd 0x773594

; Extended BPB (DOS 4.0)
DriveNumber         db 0x80
WinNTBit            db 0x00
Signature           db 0x29
VolumeID            dd 0xD105
VolumeIDString      db 'PEACHOS BOO'
SystemIDString      db 'FAT16   '


start:
    jmp 0:init ; Will set CS register to 0x7c0

init:
    cli ; Clear interrupts flag
    mov ax, 0x00
    mov ds, ax ; Set up DS register to origin
    mov es, ax ; Set up ES register to origin
    mov ss, ax ; Set up SS register to 0x00
    mov sp, 0x7c00 ; Set up SP register to origin

    sti ; Enables interrupts

.load_protected:
    cli
    lgdt[gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp CODE_SEG:load32

; GDT (Global Descriptor Table)
gdt_start:
gdt_null:
    ; 64 bits of zeros, a null descriptor
    dd 0x0
    dd 0x0

; offset 0x8
gdt_code: ; CS should point to this
    dw 0xffff ; Segment limit first 0-15 bits
    dw 0 ; Base first 0-15 bits
    db 0 ; Base 16-23 bits
    db 0x9a ; Access byte
    db 11001111b ; High 4 bit flags and low 4 bit flags
    db 0 ; Base 24-31 bits

; offset 0x10
gdt_data: ; DS, SS, ES, FS, GS
    dw 0xffff ; Segment limit first 0-15 bits
    dw 0 ; Base first 0-15 bits
    db 0 ; Base 16-23 bits
    db 0x92 ; Access byte
    db 1100111b ; High 4 bit flags and low 4 bit flags
    db 0 ; Base 24-31 bits

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

[BITS 32]
load32:
    mov eax, 1 ; Use eax to represent starting sector we want to load from (sector 1)
    mov ecx, 100 ; Use ecx to represent total number of sectors we want to load
    mov edi, 0x0100000 ; Use edi to contain the address we want to load them into
    call ata_lba_read
    jmp CODE_SEG:0x0100000

ata_lba_read:
    mov ebx, eax ; Backup the LBA
    
    ; Send the highest 8 bits of the lba to the hard disk controller
    shr eax, 24 ; Shift eax register 24 bits to the right for the highest 8 bits
    or eax, 0xE0 ; Selects the master drive
    mov dx, 0x1F6 ; The controller port that expects bits to be written to
    out dx, al ; Write out to bus
    ; Finished sending the highest 8 bits of the lba

    ; Send the total sectors to read
    mov eax, ecx
    mov dx, 0x1F2
    out dx, al ; Write out to bus
    ; Finish sending the total sectors to read

    ; Send more bits of the LBA
    mov eax, ebx ; Restoring the backup LBA
    mov dx, 0x1F3
    out dx, al ; Write out to bus
    ; Finished sending more bits of the LBA

    ; Send more bits of the LBA
    mov dx, 0x1F4
    mov eax, ebx ; Restore the backup LBA
    shr eax, 8
    out dx, al
    ; Finished sending more bits of the LBA

    ; Send upper 16 bits of the LBA
    mov dx, 0x1F5
    mov eax, ebx ; Restore the backup LBA
    shr eax, 16
    out dx, al
    ; Finished sending upper 16 bits of the LBA

    mov dx, 0x1F7
    mov al, 0x20
    out dx, al

; Read all sectors into memory
.next_sector:
    push ecx

; Checking if we need to read
.try_again:
    mov dx, 0x1F7 ; Read from port 0x1F7 into AL register
    in al, dx
    test al, 8 ; Test AL register to see if a bit is set in the bit mask
    jz .try_again ; Jump back to try again until the test doesn't fail

; We need to read 256 words at a time
    mov ecx, 256
    mov dx, 0x1F0
    rep insw ; Repeat reading a word from port 0x1F0 and storing it into beginning of kernel address section (INSW INSTRUCTION)
    pop ecx ; Restore ecx sector count
    loop .next_sector
    ; End of reading sectors into memory
    ret
 
; Fill at least 510 bytes of data, whatever bytes are not used by the program are filled with zeros with this instruction
times 510-($ - $$) db 0

; Datawrite (assemble word) 0xAA55 (is flipped due to little-endianness)
dw 0xAA55