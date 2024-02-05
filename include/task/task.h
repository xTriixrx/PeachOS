#ifndef TASK_H
#define TASK_H

#include "config.h"
#include <stdint.h>
#include "idt/idt.h"
#include "memory/paging/paging.h"

struct interrupt_frame;

struct registers
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t esp;
    uint32_t ss;
};

struct process;

struct task
{
    // The page directory of the task
    struct paging_4gb_chunk* page_directory;

    // The registers of the task when the task is not running
    struct registers registers;

    // The process of the task
    struct process* process;

    // The next task in the linked list
    struct task* next;

    // The previous task in the linked list
    struct task* prev;
};

int task_page();
struct task* task_current();
int task_free(struct task*);
struct task* task_get_next();
int task_switch(struct task*);
struct task* task_new(struct process*);

void task_run_first_ever_task();

void user_registers();
int task_page_task(struct task* task);
void task_return(struct registers* regs);
void* task_get_stack_item(struct task* task, int index);
void task_current_save_state(struct interrupt_frame* frame);
void restore_general_purpose_registers(struct registers* regs);
int copy_string_from_task(struct task* task, void* virtual, void* physical, int max);

#endif