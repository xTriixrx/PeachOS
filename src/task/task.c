#include "config.h"
#include "lib.h"
#include "task.h"
#include "kheap.h"
#include "status.h"
#include "memory.h"
#include "process.h"

// The current task that is running
struct task* current_task = 0;

// Task linked list
struct task* task_tail = 0;
struct task* task_head = 0;

struct task* task_current()
{
    return current_task;
}

int task_switch(struct task* task)
{
    current_task = task;
    paging_switch(task->page_directory);
    return 0;
}

int task_page()
{
    user_registers();
    task_switch(current_task);
    return 0;
}

void task_run_first_ever_task()
{
    if (!current_task)
    {
        panic("task_run_first_ever_task(): No current task exists!\n");
    }

    task_switch(task_head);
    task_return(&task_head->registers);
}

int task_init(struct task* task, struct process* process)
{
    memset(task, 0, sizeof(struct task));
    task->page_directory = paging_new_4gb(PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

    if (!task->page_directory)
    {
        return -EIO;
    }

    task->registers.ip = PROGRAM_VIRTUAL_ADDRESS;
    task->registers.ss = USER_DATA_SEGMENT;
    task->registers.cs = USER_CODE_SEGMENT;
    task->registers.esp = PROGRAM_VIRTUAL_STACK_ADDRESS_START;
    
    task->process = process;

    return 0;
}

struct task* task_get_next()
{
    if (!current_task->next)
    {
        return task_head;
    }

    return current_task->next;
}

static void task_list_remove(struct task* task)
{
    if (task->prev)
    {
        task->prev->next = task->next;
    }

    if (task == task_head)
    {
        task_head = task->next;
    }

    if (task == task_tail)
    {
        task_tail = task->prev;
    }

    if (task == current_task)
    {
        current_task = task_get_next();
    }
}

int task_free(struct task* task)
{
    paging_free_4gb(task->page_directory);
    task_list_remove(task);
    
    // Finally free the task data
    free(task);

    return 0;
}

struct task* task_new(struct process* process)
{
    int res = 0;
    struct task* task = kzalloc(sizeof(struct task));

    if (!task)
    {
        return ERROR(-ENOMEM);
    }

    res = task_init(task, process);
    
    if (res != ALL_OK)
    {
        task_free(task);
        return ERROR(res);
    }

    if (task_head == 0)
    {
        task_head = task;
        task_tail = task;
        current_task = task;
        return task;
    }

    task_tail->next = task;
    task->prev = task_tail;
    task_tail = task;

    return task;
}