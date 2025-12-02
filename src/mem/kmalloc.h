#ifndef __mem_kmalloc_h
#define __mem_kmalloc_h

#include <stdint.h>

#define KMALLOC_SPLITS 6

void  *kmalloc(uint32_t numBytes);
uint32_t free(uint32_t *addr);

#endif