#ifndef __mem_paging_h
#define __mem_paging_h


// with help from https://isu-rathnayaka.medium.com/develop-your-own-x86-operating-system-os-8-78a25b945dd, https://www.youtube.com/watch?v=QIGun_BGuDw, and https://wiki.osdev.org/Setting_Up_Paging

#include <stdint.h>


#define NUM_PAGES 1024
#define PAGE_FRAME_SIZE 4096
#define PAGE_SIZE 4096
#define avlBitPos 9



typedef struct {
   uint32_t entries[1024] __attribute__((aligned(4096)));
} page_table;

typedef struct {
   uint32_t entries[1024] __attribute__((aligned(4096)));
} page_directory;


// from https://www.youtube.com/watch?v=QIGun_BGuDw
typedef enum {
   PTE_PRESENT          = 0x01,
   PTE_READ_WRITE       = 0x02,
   PTE_USER             = 0x04,
   PTE_WRITE_THROUGH    = 0x08,
   PTE_CACHE_DISABLE    = 0x10,
   PTE_ACCESSED         = 0x20,
   PTE_DIRTY            = 0x40,
   PTE_PAT              = 0x80,
   PTE_GLOBAL           = 0x100,
   PTE_FRAME            = 0x7FFFF0000, // the last 12 bits
} PAGE_TABLE_FLAGS; 

typedef enum {
   PDE_PRESENT          = 0x01,
   PDE_READ_WRITE       = 0x02,
   PDE_USER             = 0x04,
   PDE_WRITE_THROUGH    = 0x08,
   PDE_CACHE_DISABLE    = 0x10,
   PDE_ACCESSED         = 0x20,
   PDE_DIRTY            = 0x40,
   PDE_PAGE_SIZE        = 0x80,
   PDE_GLOBAL           = 0x100,
   PDE_PAT              = 0x2000,
   PDE_FRAME            = 0x7FFFF0000, // the last 12 bits
} PAGE_DIR_FLAGS; 



extern void loadPageDirectory(uint32_t *);
extern void enablePaging();

void initPaging();
uint32_t *getPagesPhysAddr(uint32_t val);
int doesDirEntryExist(uint32_t addr);
uint32_t *getNewPageTable();
page_directory getPageDir(); // returns the current page dir addr
int getPageTableIndex(uint32_t addr);
void installNewPageFull(uint32_t addr, uint32_t phsyAddr);
void listPresentPages();
void setDirEntPres(uint32_t dirIndex);

// ********************************
// *         Logging Stuff        *
// ********************************
uint8_t isThereNewData();
uint32_t readLastAddr();
void disablePages();

// ********************************
// *    End of Logging Stuff      *
// ********************************

int getPageDirIndex(uint32_t addr);
int getPageFrameIndex(uint32_t addr);

void *allocNewPage();


void installPageTabEntry(uint32_t virtAddr, uint32_t physAddr);
int installPageDirEntry(page_table *table, uint16_t pos); 

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
