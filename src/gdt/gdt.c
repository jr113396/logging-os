#include "gdt.h"
#include "tss.h"
#include "mem/stdmem.h"

extern void gdt_flush(struct gdt_ptr_s *);
extern void tss_flush();
struct gdt_entry_s  gdt_entries[6];
struct gdt_ptr_s gdt_ptr;

struct TSS_struct_protected TSS;

void initGdt() {
    gdt_ptr.limit = (sizeof(struct gdt_entry_s) * 6) -1;
    gdt_ptr.base  = (uint32_t)&gdt_entries;

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

    setTSS(5, 0x10, 0); // tss

    gdt_flush((struct gdt_ptr_s *) &gdt_ptr);
    tss_flush();
}

void setTSS(uint32_t num, uint32_t SS0, uint32_t ESP0) {
    memset(&TSS, 0, sizeof(TSS));
    
    uint32_t addr = (uint32_t) &TSS;

    gdt_entries[num].base_low  = (addr & 0xFFFF); // take lower set of bits
    gdt_entries[num].base_mid  = (addr >> 16) & 0xFF; // take middle bits
    gdt_entries[num].base_high = (addr >> 24) & 0xFF; // take high bits

    gdt_entries[num].limit     = sizeof(TSS);
    gdt_entries[num].flags     = 0x40;
    gdt_entries[num].access    = 0x89;

    TSS.SS0     = SS0;
    TSS.ESP0    = ESP0;
    TSS.IOPB    = sizeof(TSS);
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