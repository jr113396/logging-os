#ifndef __mem_paging_h
#define __mem_paging_h


// with help from https://isu-rathnayaka.medium.com/develop-your-own-x86-operating-system-os-8-78a25b945dde and https://wiki.osdev.org/Setting_Up_Paging

#include <stdint.h>


#define NUM_PAGES 1024
#define PAGE_FRAME_SIZE 4096

#define PRESENT         1
#define PAGE_READONLY   0
#define PAGE_READWRITE  1
#define PAGE_USER       1
#define PAGE_KERNEL     0
#define PAGE_SIZE_4KB   0
#define PAGE_SIZE_4MB   1

#define PAGING_PRESENT      (1 << 0)
#define PAGING_WRITE        (1 << 1)
#define PAGING_SUPERVISOR   (1 << 2)

extern void loadPageDirectory(uint32_t *);
extern void enablePaging();

void initPaging();


struct page {
   unsigned int present    : 1;
   unsigned int rw         : 1; 
   unsigned int user       : 1;  
   unsigned int accessed   : 1;  
   unsigned int dirty      : 1;
   unsigned int unused     : 7; 
   unsigned int frame      : 20;
};

struct pageTable {
   struct page pages[1024] __attribute__((aligned(4096)));
};

struct page_directory {
   /**
      Array of pointers to pagetables.
   **/
   struct pageTable *tables[1024];
   /**
      Array of pointers to the pagetables above, but gives their *physical*
      location, for loading into the CR3 register.
   **/
   unsigned int tablesPhysical[1024];
   /**
      The physical address of tablesPhysical. This comes into play
      when we get our kernel heap allocated and the directory
      may be in a different location in virtual memory.
   **/
   unsigned int physicalAddr;

};

// struct PageDirectoryEntry {
//     uint8_t lowerFlags;
//     uint8_t upperFlags; // also includes bits 12-15
//     uint16_t higherAddr; // buts 31-16
//     // Flags - P, R/W, U/S, PWT, PCD, A, AVL, PS, AVL
//     // P    - Present
//     // R/W  - Read/Write
//     // U/S  - User/Supervisor
//     // PWT  - Write Through
//     // PCD  - Cache Disable
//     // A    - Accessed
//     // Avl  - Available
//     // PS   - Page Size
// } __attribute__((packed));

// struct PageTableEntry {
//     uint8_t lowerFlags;
//     uint8_t upperFlags; // also includes bits 12-15
//     uint16_t higherAddr; // buts 31-16
//     // Flags P, R/W, U/S, PWT, PCD, A, D, PAT, G, AVL
//     // P    - Present
//     // R/W  - Read/Write
//     // U/S  - User/Supervisor
//     // PWT  - Write Through
//     // PCD  - Cache Disable
//     // A    - Accessed
//     // D    - Dirty
//     // PAT  - Page Attribute Table
//     // G    - Global
//     // AVL  - Available
// } __attribute__((packed));

#endif