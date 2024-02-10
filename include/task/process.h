#ifndef PROCESS_H
#define PROCESS_H

#include "task.h"
#include "config.h"
#include <stdint.h>

struct process
{
    // The process id;
    uint16_t id;
    char filename[MAX_PATH];
    
    // The main process task
    struct task* task;

    // The memory (malloc) allocations of the process
    void* allocations[MAX_PROGRAM_ALLOCATIONS];

    // The physical pointer to the process memory
    void* ptr;

    // The physical pointer to the stack memory
    void* stack;

    // The size of the data pointed to by "ptr"
    uint32_t size;

    struct keyboard_buffer
    {
        char buffer[KEYBOARD_BUFFER_SIZE];
        int tail;
        int head;
    } keyboard;
};

struct process* process_get(int);
struct process* process_current();
int process_load(const char*, struct process**);

#endif