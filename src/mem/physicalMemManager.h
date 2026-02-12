#ifndef __mem_physical_mem_manager_h
#define __mem_physical_mem_manager_h
#include <stdint.h>

#define LENGTH_OF_BITMAP 8120
#define LENGTH_OF_LOWER_MAP 2556
uint32_t *findNextHigherFree(uint8_t numPages);
uint32_t *findNextLowerFree(uint8_t numPages);
void setHigherMemStart(uint32_t * memStartAddr);
void setLowerMem(uint32_t *start, uint32_t *end);
uint32_t returnHigherPage(uint32_t *addr);

#endif