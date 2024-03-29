#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define PAGING_CACHE_DISABLED  0b00010000
#define PAGING_WRITE_THROUGH   0b00001000
#define PAGING_ACCESS_FROM_ALL 0b00000100
#define PAGING_IS_WRITEABLE    0b00000010
#define PAGING_IS_PRESENT      0b00000001

#define PAGING_PAGE_SIZE 4096
#define PAGING_TOTAL_ENTRIES_PER_TABLE 1024

struct paging_4gb_chunk
{
    uint32_t* directory_entry;
};

void enable_paging();
bool paging_is_aligned(void*);
void* paging_align_address(void*);
uint32_t paging_get(uint32_t*, void*);
void* paging_align_to_lower_page(void*);
int paging_set(uint32_t*, void*, uint32_t);
void paging_switch(struct paging_4gb_chunk*);
void paging_free_4gb(struct paging_4gb_chunk*);
struct paging_4gb_chunk* paging_new_4gb(uint8_t);
int paging_map(struct paging_4gb_chunk*, void*, void*, int);
uint32_t* paging_4gb_chunk_get_directory(struct paging_4gb_chunk*);
int paging_map_to(struct paging_4gb_chunk*, void*, void*, void*, int);
int paging_map_range(struct paging_4gb_chunk*, void*, void*, int, int);

#endif