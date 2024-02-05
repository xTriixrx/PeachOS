#ifndef CONFIG_H
#define CONFIG_H

#define MAX_PATH 108
#define SECTOR_SIZE 512
#define MAX_PROCESSES 12
#define MAX_FILESYSTEMS 12
#define MAX_FILE_DESCRIPTORS 512
#define MAX_PROGRAM_ALLOCATIONS 1024

#define TOTAL_GDT_SEGMENTS 6

#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10

#define TOTAL_INTERRUPTS 512
#define HEAP_SIZE_BYTES 104857600 // 100 MB heap size
#define HEAP_BLOCK_SIZE 4096 // 4 KB block size
#define HEAP_START_ADDRESS 0x01000000
#define HEAP_TABLE_START_ADDRESS 0x00007E00

#define USER_DATA_SEGMENT 0x23
#define USER_CODE_SEGMENT 0x1B
#define PROGRAM_VIRTUAL_ADDRESS 0x400000
#define USER_PROGRAM_STACK_SIZE 1024 * 16 // 16 KB stack size
#define PROGRAM_VIRTUAL_STACK_ADDRESS_START 0x3FF000
#define PROGRAM_VIRTUAL_STACK_ADDRESS_END PROGRAM_VIRTUAL_STACK_ADDRESS_START - USER_PROGRAM_STACK_SIZE

#define MAX_ISR80H_COMMANDS 1024

#endif