#include "io.h"
#include "idt.h"
#include "lib.h"
#include "memory.h"
#include "config.h"
#include "kernel.h"
#include "task/task.h"

struct idt_desc idt_desc_descriptors[TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

static ISR80H_COMMAND isr80h_commands[MAX_ISR80H_COMMANDS];

extern void int21h();
extern void no_interrupt();
extern void isr80h_wrapper();
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
    idt_set(0x80, isr80h_wrapper);

    // Load the interrupt descriptor table
    idt_load(&idtr_descriptor);
}

void isr80h_register_command(int command_id, ISR80H_COMMAND command)
{
    if (command_id < 0 || command_id >= MAX_ISR80H_COMMANDS)
    {
        panic("The command is out of bounds\n");
    }

    if (isr80h_commands[command_id])
    {
        panic("You are attempting to overwrite an existing command\n");
    }

    isr80h_commands[command_id] = command;
}

void* isr80h_handle_command(int command, struct interrupt_frame* frame)
{
    void* result = 0;

    if (command < 0 || command >= MAX_ISR80H_COMMANDS)
    {
        // Invalid command
        return 0;
    }

    ISR80H_COMMAND command_func = isr80h_commands[command];
    /* 
        User land function tried executing a kernel command that the kernel is unaware of.
        This does not mean it's an error, it could be a newer userland program running on an old kernel, so we should return 0.
    */
    if (!command_func)
    {
        return 0;
    }

    result = command_func(frame);

    return result;
}

void* isr80h_handler(int command, struct interrupt_frame* frame)
{
    void* res = 0;
    kernel_page();
    
    task_current_save_state(frame);
    
    res = isr80h_handle_command(command, frame);
    
    task_page();

    return res;
}