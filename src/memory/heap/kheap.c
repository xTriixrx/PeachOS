#include "lib.h"
#include "heap.h"
#include "kheap.h"
#include "config.h"
#include "memory.h"

struct heap kernel_heap;
struct heap_table kernel_heap_table;

void heap_init()
{
    int total_table_entries = HEAP_SIZE_BYTES / HEAP_BLOCK_SIZE;
    kernel_heap_table.entries = (HEAP_BLOCK_TABLE_ENTRY*) HEAP_TABLE_START_ADDRESS;
    kernel_heap_table.total = total_table_entries;

    void* end = (void*) HEAP_START_ADDRESS + HEAP_SIZE_BYTES;
    int res = heap_create(&kernel_heap, (void*)HEAP_START_ADDRESS, end, &kernel_heap_table);

    if (res < 0)
    {
        print("Failed to create heap!\n");
    }
}

void* malloc(size_t size)
{
    return heap_malloc(&kernel_heap, size);
}

void* kzalloc(size_t size)
{
    void* ptr = malloc(size);

    if (!ptr)
    {
        return 0;
    }

    memset(ptr, 0x00, size);

    return ptr;
}

void free(void* ptr)
{
    heap_free(&kernel_heap, ptr);
}