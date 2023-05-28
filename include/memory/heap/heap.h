#ifndef HEAP_H
#define HEAP_H

#include "config.h"
#include "status.h"
#include "memory.h"
#include <stdint.h>
#include <stddef.h>

#define HEAP_BLOCK_TABLE_ENTRY_FREE 0x00
#define HEAP_BLOCK_TABLE_ENTRY_TAKEN 0x01

#define HEAP_BLOCK_HAS_NEXT 0b10000000
#define HEAP_BLOCK_IS_FIRST 0b01000000

typedef unsigned char HEAP_BLOCK_TABLE_ENTRY;

struct heap_table
{
    size_t total;
    HEAP_BLOCK_TABLE_ENTRY* entries;
};

struct heap
{
    void* saddr;
    struct heap_table* table;
};

void heap_free(struct heap*, void* ptr);
void* heap_malloc(struct heap*, size_t size);
int heap_create(struct heap*, void*, void*, struct heap_table*);

#endif