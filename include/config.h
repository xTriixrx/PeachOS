#ifndef CONFIG_H
#define CONFIG_H

#define MAX_PATH 108
#define SECTOR_SIZE 512
#define MAX_FILESYSTEMS 12
#define MAX_FILE_DESCRIPTORS 512

#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10

#define TOTAL_INTERRUPTS 512
#define HEAP_SIZE_BYTES 104857600 // 100 MB heap size
#define HEAP_BLOCK_SIZE 4096 // 4 KB block size
#define HEAP_START_ADDRESS 0x01000000
#define HEAP_TABLE_START_ADDRESS 0x00007E00

#endif