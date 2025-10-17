#ifndef stdmemh
#define stdmemh

#include <stdint.h>

void memset(void *loc, char val, uint32_t count);
void *memcpy(void *dst, void *src, uint32_t count);

#endif