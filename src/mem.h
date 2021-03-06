#pragma once
#include <stddef.h>
#include "multiboot.h"

void *memcpy(void *destination, const void *source, size_t num);
void *memset(void *destination, int value, size_t num);
void init_memory_management(multiboot_info_t *mbi);
void *alloc_block();
void free_block(void *addr);
void mark_used(void *addr);
void mark_free(void *addr);
void *malloc(size_t size);
void free(void *addr);