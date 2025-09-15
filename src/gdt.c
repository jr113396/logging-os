#include "gdt.h"

extern void gdt_flush(struct gdt_ptr_s *addr_t);

struct gdt_entry_s  gdt_entries[5];
struct gdt_ptr_s gdt_ptr;

void initGdt() {
    gdt_ptr.limit = (sizeof(struct gdt_entry_s) * 5) -1;
    gdt_ptr.base  = (unsigned int)&gdt_entries;

    setGdtGate(0,0,0,0,0); // required to be all 0s -> NULL segment

    // BASE                 = 0 -> starts at addr 0
    // LIMIT + granularity  = 4kb paging
    // ACCCESS              = 9A -> 1001 1010
    //                              ^~~^ ^^^^
    //                             /  ^| |||Access bit -> nothing has accessed this
    //   Present Bit - Need to be     || |||
    //   0                           / | ||R/W -> Read Access is allowed
    //      DPL of 0 - Kernel segment  | ||
    //                                 | |Direction bit -> Grows up
    //                                 / |
    //                Code segment so 1  Execution bit, code segment so need to be 1
    setGdtGate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // kernel code segment
    setGdtGate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // kernel data segment
    setGdtGate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // user code segment
    setGdtGate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // user data segment

    gdt_flush(&gdt_ptr);
}

void setGdtGate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity) {
    gdt_entries[num].base_low  = (base & 0xFFFF); // take lower set of bits
    gdt_entries[num].base_mid  = (base >> 16) & 0xFF; // take middle bits
    gdt_entries[num].base_high = (base >> 24) & 0xFF; // take high bits

    gdt_entries[num].limit     = (limit & 0xFFFF); // take lower 16 bits of limit 
    gdt_entries[num].flags     = (limit >> 16) & 0x0F; // higher 4 bits of limit
    gdt_entries[num].flags    |= (granularity & 0xF0); // set flags -> only 4 bits
    
    gdt_entries[num].access    = access;
}