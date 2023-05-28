#include "fs/pparser.h"
#include "io.h"
#include "idt.h"
#include "lib.h"
#include "disk.h"
#include "kheap.h"
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

    struct disk_stream* stream = diskstreamer_new(0);
    diskstreamer_seek(stream, 0x201);
    unsigned char c = 0;
    diskstreamer_read(stream, &c, 1);
}