#include "idt.h"

struct idt_entry_s idt_entries[256];
struct idt_ptr_s idt_ptr;

extern void idt_flush(uint32_t);

void setIDT() {
    idt_ptr.limit = sizeof(struct idt_entry_s) * 256 - 1;
    idt_ptr.base = (uint32_t) &idt_entries;

    memset(&idt_entries, 0, sizeof(idt_entry_s) * 256); //reset all values to 0

    // PICs sit at 
    //          0x20 commands, 0x21 data
    //          0xA0 commands, 0xA1 data
}

void setIDTGate(uint8_t num, uint32_t offset, uint16_t segment, uint8_t flags) {

}