#include "gdt/gdt.h"
#include "io.h"
#include "gdt.h"
#include "idt.h"
#include "lib.h"
#include "file.h"
#include "disk.h"
#include "kheap.h"
#include "config.h"
#include "memory.h"
#include "string.h"
#include "paging.h"
#include "kernel.h"
#include <stdint.h>
#include "pparser.h"
#include "diskstreamer.h"

static struct paging_4gb_chunk* kernel_chunk = 0;

struct gdt gdt_real[TOTAL_GDT_SEGMENTS];
struct gdt_structured gdt_structured[TOTAL_GDT_SEGMENTS] = {
    {.base = 0x00, .limit = 0x00, .type = 0x00 }, // NULL Segment
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0x9A }, // Kernel Code Segment
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0x92 }, // Kernel Data Segment
};

void kernel_main()
{
    terminal_initialize();
    
    const char* hello = "Hello World!\nThis is Vincent!\tHow are you?\n";
    print(hello);

    memset(gdt_real, 0x00, sizeof(gdt_real));
    gdt_structured_to_gdt(gdt_real, gdt_structured, TOTAL_GDT_SEGMENTS);
    
    // Load the GDT
    gdt_load(gdt_real, sizeof(gdt_real));

    // Initialize the heap
    heap_init();

    // Initialize the file systems
    fs_init();

    // Search and initialize the disks
    disk_search_and_init();

    // Initialize the interrupt descriptor table
    idt_init();

    // Setup paging
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

    // Switch to kernel paging chunk
    paging_switch(paging_4gb_chunk_get_directory(kernel_chunk));

    // Enable Paging
    enable_paging();

    // Enable the system interrupts
    enable_interrupts();

    int fd = fopen("0:/hello.txt", "r");
    
    if (fd)
    {
        struct file_stat s;
        fstat(fd, &s);
        print("\nWe opened hello.txt\n");
        char buf[14];
        fseek(fd, 2, SEEK_SET);
        fread(buf, 11, 1, fd);
        buf[13] = 0x00;
        print(buf);

        fclose(fd);

        print("Testing close!\n");
    }

    while(1) {}
}