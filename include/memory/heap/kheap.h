#ifndef KHEAP_H
#define KHEAP_H

#include <stdint.h>
#include <stddef.h>

void heap_init();
void free(void*);
void* malloc(size_t);
void* kzalloc(size_t);

#endif