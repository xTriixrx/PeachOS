#include "io.h"
#include "tss.h"
#include "gdt.h"
#include "idt.h"
#include "lib.h"
#include "task.h"
#include "file.h"
#include "disk.h"
#include "kheap.h"
#include "config.h"
#include "memory.h"
#include "string.h"
#include "paging.h"
#include "kernel.h"
#include <stdint.h>
#include "status.h"
#include "process.h"
#include "pparser.h"
#include "diskstreamer.h"
#include "isr80h/isr80h.h"
#include "keyboard/keyboard.h"

static struct paging_4gb_chunk* kernel_chunk = 0;

void kernel_page()
{
    kernel_registers();
    paging_switch(kernel_chunk);
}

struct tss tss;
struct gdt gdt_real[TOTAL_GDT_SEGMENTS];
struct gdt_structured gdt_structured[TOTAL_GDT_SEGMENTS] = {
    {.base = 0x00, .limit = 0x00, .type = 0x00 }, // NULL Segment
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0x9A }, // Kernel Code Segment
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0x92 }, // Kernel Data Segment
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0xF8 }, // User Code Segment
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0xF2}, // User Data Segment
    {.base = (uint32_t) &tss, .limit = sizeof(tss), .type = 0xE9}, // TSS Segment
};

void kernel_main()
{
    terminal_initialize();

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

    // Set up the TSS
    memset(&tss, 0x00, sizeof(tss));
    tss.esp0 = 0x600000; // Where Kernel Stack is located
    tss.ss0 = KERNEL_DATA_SELECTOR;

    // Load the TSS
    tss_load(0x28); // 0x28 is the offset in the GDT
    
    // Setup paging
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

    // Switch to kernel paging chunk
    paging_switch(kernel_chunk);

    // Enable Paging
    enable_paging();

    // Register kernel commands
    isr80h_register_commands();

    // Initialize all the system keyboards
    keyboard_init();

    struct process* process = 0;
    int res = process_load("0:/blank.bin", &process);
    
    if (res != ALL_OK)
    {
        panic("Failed to load blank.bin!");
    }

    task_run_first_ever_task();

    // Enable the system interrupts
    //enable_interrupts();

    while(1) {}
}