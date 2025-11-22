#ifndef __mem_physical_mem_manager_h
#define __mem_physical_mem_manager_h
#include <stdint.h>

#define LENGTH_OF_BITMAP 8120
#define LENGTH_OF_LOWER_MAP 2556
uint32_t *findNextHigherFree();
uint32_t *findNextLowerFree();
void setMemStart(uint32_t * memStartAddr);

#endif