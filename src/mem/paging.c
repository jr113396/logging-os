#include "paging.h"
#include <drivers/serial/serial.h>
#include <stdio.h>
#include <interupts/idt.h>

// with help from https://wiki.osdev.org/Setting_Up_Paging and https://wiki.osdev.org/Paging and https://isu-rathnayaka.medium.com/develop-your-own-x86-operating-system-os-8-78a25b945dde

static uint32_t pageDirectory[NUM_PAGES] __attribute__((aligned(PAGE_FRAME_SIZE)));
static uint32_t pageTableOne[NUM_PAGES] __attribute__((aligned(PAGE_FRAME_SIZE)));

page_directory *currentPageDir = 0;
physical_addr currentPageDirAddr = 0;

// with help from https://www.youtube.com/watch?v=QIGun_BGuDw

int getPageDirIndex(uint32_t addr) {
    return addr >> 22; // get what page entry contains this addr 0-1023
}

int getPageTableIndex(uint32_t addr) {
    return ((addr >> 12) & 1023); // get what page frame contains this addr 0-1023 - removing the flags
}

int getPagesPhysAddr(uint32_t *entry) {
    return (*entry & ~0xFFF); // remove the flags from the entry and return 
}

pt_entry *getPageTableEntry(page_table *table, virtual_addr addr) {
    if (table) {
        // check that the table is not null so is valid
        return &table->entries[getPageTableIndex(addr)];
    }
    return 0;
}

pd_entry *getPageDirectoryEntry(page_directory *dir, virtual_addr addr) {
    if (dir) {
        // check that the directory is not null so is valid
        return &dir->entries[getPageTableIndex(addr)]; // this is intentionally getPageTableIndex
    }
    return 0;
}

// gets the page in the currently used directory by addr
pt_entry *getPageInCurrent(virtual_addr addr) {
    page_directory *dir = currentPageDir;
    
    pd_entry *dirEntry = &dir->entries[getPageDirIndex(addr)];

    page_table *dirTable = (page_table *)getPagesPhysAddr(dirEntry);
    
    pt_entry *page = &dirTable->entries[getPageTableIndex(addr)];
    
    return page;
}

void pageFaultHandler(struct InterruptRegisters *regs) {
    kprintf("Page Fault\r\n");
    kprintf("Processor Halting\r\n");
    asm ( "hlt;\n\t" );

}

void initPaging() {
    for (int i = 0; i < NUM_PAGES; i++) {
        // set each entry to have no present bit to disable it
        pageDirectory[i] = 0x00000002; // | PAGING_SUPERVISOR | PAGING_WRITE; // note no present bit
    }
    
    for (int i = 0; i < NUM_PAGES; i++) {
        // set to present and r/w and at supervisor level
        pageTableOne[i] = (i * 0x1000) | 3; // | PAGING_SUPERVISOR | PAGING_WRITE | PAGING_PRESENT;
    }

    // put page table into page directory
    pageDirectory[0] = ((unsigned int)pageTableOne) | 3;// | PAGING_SUPERVISOR | PAGING_WRITE | PAGING_PRESENT;

    install_isr_handler (14, &pageFaultHandler); 

    loadPageDirectory(pageDirectory);
    enablePaging();
}
