#include "heap.h"

static int heap_validate_table(void* ptr, void* end, struct heap_table* table)
{
    int res = 0;

    size_t table_size = (size_t) (end - ptr);
    size_t total_blocks = table_size / HEAP_BLOCK_SIZE;

    if (table->total != total_blocks)
    {
        res = -EINVARG;
        return res;
    }

    return res;
}

static int heap_validate_alignment(void* ptr)
{
    return ((unsigned int) ptr % HEAP_BLOCK_SIZE) == 0;
}

int heap_create(struct heap* heap, void* ptr, void* end, struct heap_table* table)
{
    int res = ALL_OK;

    if (!heap_validate_alignment(ptr) || !heap_validate_alignment(end))
    {
        res = -EINVARG;
        return res;
    }

    memset(heap, 0, sizeof(struct heap));
    
    heap->saddr = ptr;
    heap->table = table;

    res = heap_validate_table(ptr, end, table);
    
    if (res < 0)
    {
        return res;
    }

    size_t table_size = sizeof(HEAP_BLOCK_TABLE_ENTRY) * table->total;
    memset(table->entries, HEAP_BLOCK_TABLE_ENTRY_FREE, table_size);

    return res;
}

static uint32_t heap_align_value(uint32_t val)
{
    uint32_t alloc_val = 0;

    if ((val % HEAP_BLOCK_SIZE) == 0)
    {
        return val;
    }

    alloc_val = (val - (val % HEAP_BLOCK_SIZE)) + HEAP_BLOCK_SIZE;

    return alloc_val;
}

static int heap_get_entry_type(HEAP_BLOCK_TABLE_ENTRY entry)
{
    return entry & 0x0F;
}

int heap_get_start_block(struct heap* heap, uint32_t total_blocks)
{
    int bc = 0; // Stores current block number
    int bs = -1; // Stores block start number
    struct heap_table* table = heap->table;

    for (size_t i = 0; i < table->total; i++)
    {
        if (heap_get_entry_type(table->entries[i]) != HEAP_BLOCK_TABLE_ENTRY_FREE)
        {
            bc = 0;
            bs = -1;
            continue;
        }

        // If this is the first block
        if (bs == -1)
        {
            bs = i;
        }

        bc++;

        if (bc == total_blocks)
        {
            break;
        }
    }

    if (bs == -1)
    {
        return -ENOMEM;
    }

    return bs;
}

void* heap_block_to_address(struct heap* heap, int block)
{
    return heap->saddr + (block * HEAP_BLOCK_SIZE);
}

void heap_mark_blocks_taken(struct heap* heap, int start_block, int total_blocks)
{
    int end_block = (start_block + total_blocks) - 1;

    HEAP_BLOCK_TABLE_ENTRY entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN | HEAP_BLOCK_IS_FIRST;

    if (total_blocks > 1)
    {
        entry |= HEAP_BLOCK_HAS_NEXT;
    }

    for (int i = start_block; i <= end_block; i++)
    {
        heap->table->entries[i] = entry;
        
        // Reset entry as an entry taken flag
        entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN;
        
        if (i != end_block - 1)
        {
            entry |= HEAP_BLOCK_HAS_NEXT;
        }        
    }
}

void* malloc_blocks(struct heap* heap, uint32_t total_blocks)
{
    void* address = 0;

    int start_block = heap_get_start_block(heap, total_blocks);

    if (start_block < 0)
    {
        return address;
    }

    address = heap_block_to_address(heap, start_block);

    // Mark the blocks as taken
    heap_mark_blocks_taken(heap, start_block, total_blocks);
    
    return address;
}

/**
 * Get relative offset by subtracting provided address by start address of heap, then divide the relative offset by the heap block
 * size to get the starting block number of some memory address.
 * 
 * @param heap 
 * @param address 
 * @return int 
 */
int heap_address_to_block(struct heap* heap, void* address)
{
    return (address - heap->saddr) / HEAP_BLOCK_SIZE;
}

void heap_mark_blocks_free(struct heap* heap, int starting_block)
{
    struct heap_table* table = heap->table;

    for (size_t i = starting_block; i < table->total; i++)
    {
        HEAP_BLOCK_TABLE_ENTRY entry = table->entries[i];
        table->entries[i] = HEAP_BLOCK_TABLE_ENTRY_FREE;

        if (!(entry & HEAP_BLOCK_HAS_NEXT))
        {
            break;
        }
    }
}

void* heap_malloc(struct heap* heap, size_t size)
{
    size_t aligned_size = heap_align_value(size);
    uint32_t total_blocks = aligned_size / HEAP_BLOCK_SIZE;

    return malloc_blocks(heap, total_blocks);
}

void heap_free(struct heap* heap, void* ptr)
{
    heap_mark_blocks_free(heap, heap_address_to_block(heap, ptr));
}