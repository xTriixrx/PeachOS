#include "io.h"
#include "idt.h"
#include "lib.h"
#include "memory.h"
#include "config.h"

struct idt_desc idt_desc_descriptors[TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

extern void int21h();
extern void no_interrupt();
extern void idt_load(struct idtr_desc* ptr);

void int21h_handler()
{
    print("Keyboard pressed!\n");
    outb(0x20, 0x20);
}

void no_interrupt_handler()
{
    outb(0x20, 0x20);
}

void idt_zero()
{
    print("Divide by zero error\n");
}

void idt_set(int itr_no, void* addr)
{
    struct idt_desc* desc = &idt_desc_descriptors[itr_no];
    desc->loffset = (uint32_t) addr & 0x0000FFFF;
    desc->selector = KERNEL_CODE_SELECTOR;
    desc->zero = 0x00;
    desc->type_attr = 0xEE;
    desc->hoffset = (uint32_t) addr >> 16;
}

void idt_init()
{
    memset(idt_desc_descriptors, 0, sizeof(idt_desc_descriptors));
    idtr_descriptor.limit = sizeof(idt_desc_descriptors) - 1;
    idtr_descriptor.base = (uint32_t) idt_desc_descriptors;

    for (int i = 0; i < TOTAL_INTERRUPTS; i++)
    {
        idt_set(i, no_interrupt);
    }

    idt_set(0, idt_zero);
    idt_set(0x21, int21h);

    // Load the interrupt descriptor table
    idt_load(&idtr_descriptor);
}