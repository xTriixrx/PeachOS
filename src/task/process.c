#include "lib.h"
#include "file.h"
#include "task.h"
#include "kheap.h"
#include "paging.h"
#include "config.h"
#include "memory.h"
#include "status.h"
#include "string.h"
#include "process.h"
#include "loader/formats/elfloader.h"
#include <elf.h>

struct process* current_process = 0;
static struct process* processes[MAX_PROCESSES] = {};

static void process_init(struct process* process)
{
    memset(process, 0, sizeof(struct process));
}

struct process* process_current()
{
    return current_process;
}

struct process* process_get(int pid)
{
    if (pid < 0 || pid >= MAX_PROCESSES)
    {
        return NULL;
    }

    return processes[pid];
}

int process_switch(struct process* process)
{
    current_process = process;
    
    return 0;
}

static int process_load_binary(const char* filename, struct process* process)
{
    int res = 0;
    int fd = fopen(filename, "r");

    if (!fd)
    {
        return -EIO;
    }

    struct file_stat stat;
    res = fstat(fd, &stat);

    if (res != ALL_OK)
    {
        fclose(fd);
        return res;
    }

    void* program_data_ptr = kzalloc(stat.filesize);

    if (!program_data_ptr)
    {
        fclose(fd);
        free(program_data_ptr);
        return -ENOMEM;
    }

    if (fread(program_data_ptr, stat.filesize, 1, fd) != 1)
    {
        fclose(fd);
        free(program_data_ptr);
        return -EIO;
    }

    process->filetype = PROCESS_FILE_TYPE_BINARY;
    process->ptr = program_data_ptr;
    process->size = stat.filesize;

    fclose(fd);

    return res;
}

static int process_load_elf(const char* filename, struct process* process)
{
    int res = 0;
    struct elf_file* elf_file = 0;
    res = elf_load(filename, &elf_file);

    if (ISERR(res))
    {
        return res;
    }

    process->filetype = PROCESS_FILE_TYPE_ELF;
    process->elf_file = elf_file;

    return res;
}

static int process_load_data(const char* filename, struct process* process)
{
    int res = 0;
    res = process_load_elf(filename, process);

    if (res == -EINFORMAT)
    {
        res = process_load_binary(filename, process);
    }

    return res;
}

static int process_map_elf(struct process* process)
{
    int res = 0;

    struct elf_file* elf_file = process->elf_file;
    struct elf_header* header = elf_header(elf_file);
    struct elf32_phdr* phdrs = elf_pheader(header);

    for (int i = 0; i < header->e_phnum; i++)
    {
        struct elf32_phdr* phdr = &phdrs[i];
        void* phdr_physical_address = elf_phdr_physical_address(elf_file, phdr);
        int flags = PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL;

        if (phdr->p_flags & PF_W)
        {
            flags |= PAGING_IS_WRITEABLE;
        }

        res = paging_map_to(process->task->page_directory, paging_align_to_lower_page((void*) phdr->p_vaddr),
            paging_align_to_lower_page(phdr_physical_address), paging_align_address(phdr_physical_address + phdr->p_filesz), flags);

        if (ISERR(res))
        {
            break;
        }
    }

    return res;
}

int process_map_binary(struct process* process)
{
    int res = 0;

    paging_map_to(process->task->page_directory, (void*) PROGRAM_VIRTUAL_ADDRESS,
        process->ptr, paging_align_address(process->ptr + process->size),
        PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE);
    
    return res;
}

int process_map_memory(struct process* process)
{
    int res = 0;

    switch (process->filetype)
    {
        case PROCESS_FILE_TYPE_ELF:
            res = process_map_elf(process);
        break;

        case PROCESS_FILE_TYPE_BINARY:
            res = process_map_binary(process);
        break;

        default:
            panic("process_map_memory: Invalid filetype.\n");
    }

    if (res < 0)
    {
        return res;
    }

    // Finally map the stack
    paging_map_to(process->task->page_directory, (void*) PROGRAM_VIRTUAL_STACK_ADDRESS_END,
        process->stack, paging_align_address(process->stack + USER_PROGRAM_STACK_SIZE),
        PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE);

    return res;
}

int process_get_free_slot()
{
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (processes[i] == 0)
        {
            return i;
        }
    }

    return -EISTKN;
}

int process_load_for_slot(const char* filename, struct process** process, int process_slot)
{
    int res = 0;
    struct task* task = 0;
    struct process* _process;
    void* program_stack_ptr = 0;

    if (process_get(process_slot) != 0)
    {
        return -EISTKN;
    }

    _process = kzalloc(sizeof(struct process));

    if (!_process)
    {
        return -ENOMEM;
    }

    process_init(_process);
    res = process_load_data(filename, _process);

    if (res < 0)
    {
        free(_process);
        return res;
    }

    program_stack_ptr = kzalloc(USER_PROGRAM_STACK_SIZE);

    if (!program_stack_ptr)
    {
        free(_process);
        return -ENOMEM;
    }

    strncpy(_process->filename, filename, sizeof(_process->filename));
    _process->stack = program_stack_ptr;
    _process->id = process_slot;

    // Create a task
    task = task_new(_process);

    if (ERROR_I(task) == 0)
    {
        task_free(_process->task);
        free(program_stack_ptr);
        free(_process);
        return ERROR_I(task);
    }

    _process->task = task;

    res = process_map_memory(_process);

    if (res < 0)
    {
        task_free(_process->task);
        free(program_stack_ptr);
        free(_process);
        return res;
    }

    *process = _process;

    // Add the process to the array
    process[process_slot] = _process;

    return res;
}

int process_load(const char* filename, struct process** process)
{
    int res = 0;
    int process_slot = process_get_free_slot();

    if (process_slot < 0)
    {
        return -EISTKN;
    }

    res = process_load_for_slot(filename, process, process_slot);
    
    return res;
}

int process_load_switch(const char* filename, struct process** process)
{
    int res = process_load(filename, process);

    if (res == 0)
    {
        process_switch(*process);
    }

    return res;
}