#ifndef __mem_paging_h
#define __mem_paging_h

#include <stdint.h>

#define PAGING_PRESENT      (1 << 0)
#define PAGING_WRITE        (1 << 1)
#define PAGING_SUPERVISOR   (1 << 2)

extern void loadPageDirectory(uint32_t *);
extern void enablePaging();

void initPaging();


struct PageDirectoryEntry {
    uint8_t lowerFlags;
    uint8_t upperFlags; // also includes bits 12-15
    uint16_t higherAddr; // buts 31-16
    // Flags - P, R/W, U/S, PWT, PCD, A, AVL, PS, AVL
    // P    - Present
    // R/W  - Read/Write
    // U/S  - User/Supervisor
    // PWT  - Write Through
    // PCD  - Cache Disable
    // A    - Accessed
    // Avl  - Available
    // PS   - Page Size
} __attribute__((packed));

struct PageTableEntry {
    uint8_t lowerFlags;
    uint8_t upperFlags; // also includes bits 12-15
    uint16_t higherAddr; // buts 31-16
    // Flags P, R/W, U/S, PWT, PCD, A, D, PAT, G, AVL
    // P    - Present
    // R/W  - Read/Write
    // U/S  - User/Supervisor
    // PWT  - Write Through
    // PCD  - Cache Disable
    // A    - Accessed
    // D    - Dirty
    // PAT  - Page Attribute Table
    // G    - Global
    // AVL  - Available
} __attribute__((packed));


#endif