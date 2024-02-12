#ifndef ELFLOADER_H
#define ELFLOADER_H

#include "elf.h"
#include "config.h"
#include <stdint.h>
#include <stddef.h>

struct elf_file
{
    char filename[MAX_PATH];
    int in_memory_size;
    
    /**
     * The physical memory address that this elf file is loaded at
     */
    void* elf_memory;

    /**
     * The virtual base address of this binary
     */
    void* virtual_base_address;

    /**
     * The ending virtual address this binary
     */
    void* virtual_end_address;

    /**
     * The physical base address of this binary
     */
    void* physical_base_address;

    /**
     * The physical end address of this binary
     */
    void* physical_end_address;
};

void elf_close(struct elf_file*);
void* elf_memory(struct elf_file*);
void* elf_virtual_end(struct elf_file*);
void* elf_virtual_base(struct elf_file*);
void* elf_physical_end(struct elf_file*);
void* elf_physical_base(struct elf_file*);
int elf_load(const char*, struct elf_file**);
struct elf_header* elf_header(struct elf_file*);
struct elf32_shdr* elf_sheader(struct elf_header*);
struct elf32_phdr* elf_pheader(struct elf_header*);
struct elf32_shdr* elf_section(struct elf_header*, int);
struct elf32_phdr* elf_program_header(struct elf_header*, int);
void* elf_phdr_physical_address(struct elf_file*, struct elf32_phdr*);

#endif