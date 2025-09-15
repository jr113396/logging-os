// with lots of help from https://www.youtube.com/watch?v=jwulDRMQ53I and https://wiki.osdev.org/GDT_Tutorial 

#ifndef gdth
#define gdth

# include "stdint.h"
struct gdt_entry_s {
    uint16_t limit;
    uint16_t base_low; // first 15 bits
    uint8_t base_mid; // mid bits
    uint8_t access;
    uint8_t flags;
    uint8_t base_high; // last bits
} __attribute__((packed));

struct gdt_ptr_s {
    uint16_t limit;
    unsigned int base;
} __attribute__((packed));



void setGdtGate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity);

void initGdt();

#endif