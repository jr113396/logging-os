#ifndef idth
#define idth

#include "../stdint.h"
#include "../mem/stdmem.h"

struct idt_value_s {
    uint16_t offset_low;
    uint16_t segment_selector;
    uint8_t  resvd;
    uint8_t  flags;
    uint16_t offset_high;
} __attribute__((packed));

struct idt_ptr_s {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

void setIDT();
void setIDTGate(uint8_t num, uint32_t offset, uint16_t segment, uint8_t flags);

#endif