#include "io.h"
#include "idt.h"
#include "lib.h"
#include "file.h"
#include "disk.h"
#include "kheap.h"
#include "string.h"
#include "paging.h"
#include "kernel.h"
#include <stdint.h>
#include "pparser.h"
#include "diskstreamer.h"

static struct paging_4gb_chunk* kernel_chunk = 0;

void kernel_main()
{
    terminal_initialize();
    
    const char* hello = "Hello World!\nThis is Vincent!\tHow are you?\n";
    print(hello);

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
        print("\nWe opened hello.txt\n");
        char buf[14];
        fseek(fd, 2, SEEK_SET);
        fread(buf, 11, 1, fd);
        buf[13] = 0x00;
        print(buf);
    }

    while(1) {}
}