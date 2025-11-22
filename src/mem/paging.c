#include "paging.h"
#include <drivers/serial/serial.h>
#include <stdio.h>
#include <interupts/idt.h>
#include "physicalMemManager.h"

// with help from https://wiki.osdev.org/Setting_Up_Paging and https://wiki.osdev.org/Paging and https://isu-rathnayaka.medium.com/develop-your-own-x86-operating-system-os-8-78a25b945dde

// static uint32_t pageDirectory[NUM_PAGES] __attribute__((aligned(PAGE_FRAME_SIZE))) = {0};
// static uint32_t pageTableOne[NUM_PAGES] __attribute__((aligned(PAGE_FRAME_SIZE))) = {0}; // gives the first 4194304 bytes -> (1024 * 4) * 1024
//                                                                                          //                                   ^             ^
                                                                                         //                                   |             |
                                                                                         //           Size of a page in bytes ┘          Number of pages in a table

page_directory pageDirectory;
page_table pageTableOne;  

// with help from https://www.youtube.com/watch?v=QIGun_BGuDw

int getPageDirIndex(uint32_t addr) {
    return addr >> 22; // get what page entry contains this addr 0-1023
}

int getPageTableIndex(uint32_t addr) {
    return ((addr >> 12) & 1023); // get what page frame contains this addr 0-1023 - removing the flags
}

int getPagesPhysAddr(uint32_t val) {
    return (val & ~0xFFF); // remove the flags from the entry and return 
}

int doesDirEntryExist(uint32_t addr) {
    uint16_t index = getPageDirIndex(addr);
    sprintf("[doesDirEntryExist] Found index for addr: %d\r\n", addr); 
    uint32_t pageDirEntry = pageDirectory.entries[index];

    sprintf("[doesDirEntryExist] entry has value %d - is Present %d\r\n", pageDirEntry, pageDirEntry & PDE_PRESENT);
    return pageDirEntry & PDE_PRESENT;
}

void findCorrectPageLocation(uint32_t *addr) {
    // pageTable = currentPageDir[pageDirIndex]; // -> thing inside of pageDir at pageDirIndex
    sprintf("");
    //                     ┌──────────┐
    //                     │   Page   │
    //                     │  Direc   │     -> Addr / 1024 / 1024 / 4 
    //                     │  Entry   │      
    //                     │   *1024  │
    //                     └──────────┘
    //                          ||
    //                          ||      
    //                          ||
    //                          \/
    //                     ┌──────────┐           
    //                     │   Page   │           
    //                     │  Table   │     -> 
    //                     │  Entry   │           
    //                     │   *1024  │           
    //                     └──────────┘  
    //                          ||
    //                          ||
    //                          ||
    //                          \/     
    //                     ┌──────────┐
    //                     │          │
    //                     │  page    │
    //                     │ 4 * 1024 │     -> Physical Address
    //                     │          │
    //                     └──────────┘

} 

void pageDirFlagsBits(uint32_t dir) {
    sprintf("\r\n");
    sprintf("******************************\r\n");
    sprintf("*                            *\r\n");
    sprintf("*       DIR FLAGS            *\r\n");
    sprintf("*                            *\r\n");
    sprintf("******************************\r\n");

    if ((dir & PDE_PRESENT) != 0)
        sprintf("Dir Present\r\n");
    else
        sprintf("Dir Not Present\r\n");

    if ((dir & PDE_READ_WRITE) != 0)
        sprintf("Dir Read/Write Access\r\n");
    else
        sprintf("Dir Read Only\r\n");

    if ((dir & PDE_USER) != 0)
        sprintf("Dir Access By All\r\n");
    else
        sprintf("Dir SuperUser Only\r\n");

    if ((dir & PDE_WRITE_THROUGH) != 0)
        sprintf("Dir Write Through Enabled\r\n");
    else
        sprintf("Dir Write Back Enabled\r\n");

    if ((dir & PDE_CACHE_DISABLE) != 0)
        sprintf("Dir Page Cache Disable\r\n");
    else
        sprintf("Dir Page Cache Enabled\r\n");

    if ((dir & PDE_ACCESSED) != 0)
        sprintf("Dir Accessed\r\n");
    else
        sprintf("Dir Not Accessed\r\n");

    if ((dir & PDE_DIRTY) != 0)
        sprintf("Dir Dirty\r\n");
    else
        sprintf("Dir Not Dirty\r\n");

    if ((dir & PDE_PAGE_SIZE) != 0)
        sprintf("Dir 4MB page\r\n");
    else
        sprintf("Dir 4KB page\r\n");

    if ((dir & PDE_GLOBAL) != 0)
        sprintf("Dir Global\r\n");
    else
        sprintf("Dir Not Global\r\n");

    if ((dir & PDE_PAT) != 0)
        sprintf("Dir PAT\r\n");
    else
        sprintf("Dir Not PAT\r\n");

    sprintf("\r\n\r\n");
}

void pageTableFlagsBits(uint32_t tab) {
    sprintf("\r\n");
    sprintf("******************************\r\n");
    sprintf("*                            *\r\n");
    sprintf("*       TAB FLAGS            *\r\n");
    sprintf("*                            *\r\n");
    sprintf("******************************\r\n");
    sprintf("Tab Bits: %ub\r\n", tab);
    if ((tab & PTE_PRESENT) != 0)
        sprintf("Tab Present\r\n");
    else
        sprintf("Tab Not Present\r\n");

    if ((tab & PTE_READ_WRITE) != 0)
        sprintf("Tab Read/Write Access\r\n");
    else
        sprintf("Tab Read Only\r\n");

    if ((tab & PTE_USER) != 0)
        sprintf("Tab Access By All\r\n");
    else
        sprintf("Tab SuperUser Only\r\n");

    if ((tab & PTE_WRITE_THROUGH) != 0)
        sprintf("Tab Write Through Enabled\r\n");
    else
        sprintf("Tab Write Back Enabled\r\n");

    if ((tab & PTE_CACHE_DISABLE) != 0)
        sprintf("Tab Page Cache Disable\r\n");
    else
        sprintf("Tab Page Cache Enabled\r\n");

    if ((tab & PTE_ACCESSED) != 0)
        sprintf("Tab Accessed\r\n");
    else
        sprintf("Tab Not Accessed\r\n");

    if ((tab & PTE_DIRTY) != 0)
        sprintf("Tab dirty\r\n");
    else
        sprintf("Tab Not dirty\r\n");

    if ((tab & PTE_GLOBAL) != 0)
        sprintf("Tab Global\r\n");
    else
        sprintf("Tab Not Global\r\n");

    sprintf("\r\n\r\n");
}

void pageFaultErrorBits(uint32_t error) {
    sprintf("\r\n");
    sprintf("******************************\r\n");
    sprintf("*                            *\r\n");
    sprintf("*       Page Fault Bits      *\r\n");
    sprintf("*                            *\r\n");
    sprintf("******************************\r\n");
    sprintf("%032b\r\n", error);

    sprintf("Present Bit \t\t\t -> ");
    if ((error & (1 << 0)) != 0)
        sprintf("Page Protection Violation\r\n");
    else
        sprintf("Non Present Page\r\n");

    sprintf("Write Bit \t\t\t -> ");
    if ((error & (1 << 1)) != 0)
        sprintf("Write Access\r\n");
    else
        sprintf("Read Access\r\n");

    if ((error & (1 << 2)) != 0)
        sprintf("User Bit \t\t\t -> CPL = 3\r\n");

    if ((error & (1 << 3)) != 0)
        sprintf("Reserved Write Bit \t\t -> One or more page dir entries contain resvd bits set to 1\r\n");

    if ((error & (1 << 4)) != 0)
        sprintf("Instruction Fetch Bit \t\t -> Page Fault caused by instruction fetch\r\n");

    if ((error & (1 << 5)) != 0)
        sprintf("Protection Key Bit \t\t -> Protection key violation\r\n");

    if ((error & (1 << 6)) != 0)
        sprintf("Shadow Stack Bit \t\t -> Shadow Stack access\r\n");
    
    if ((error & (1 <<7)) != 0)
        sprintf("Software Guard Extensions \t -> Fault due SGX violation -> not related to normal pagin \r\n");

    sprintf("\r\n\r\n");
}

uint32_t getNewPageTable() {
    sprintf("Getting pages from higher mem\r\n");
    uint32_t *addr = findNextHigherFree();
    if (addr == 0) {
        sprintf("Returned addr == 0, should prob panic AAAAA!\r\n");
        return 0;
    }
    return addr; // return the address for it to be used 
    // return pageTable2;
    // return 0;
    // return &pageTableTwo;
}

int installPageDirEntry(page_table *table, uint16_t pos) {
    if (pos > 1024) {
        sprintf("Install page Dir got handed pos greater than 1024\r\n");
        return 0;
    }
    // install a page directory entry at a position in the page dir entry
    sprintf("Pos: %d\r\n", pos);
    sprintf("Got table value: %ud\r\n", (table->entries));
    pageDirectory.entries[pos] = ((uint32_t)(table->entries)) | PDE_USER | PDE_READ_WRITE | PDE_PRESENT;
    
    return 1;
}

void installPageTabEntry(uint32_t virtAddr, uint32_t physAddr) {
    // virtAddr - where the program thinks its trying to access
    // physAddr - where the data is actually going to be stored
    uint16_t dirPos = getPageDirIndex(virtAddr),
             tabPos = getPageTableIndex(virtAddr);

    sprintf("Page Dir Entry Val: %d - addr %d\r\n", pageDirectory.entries[dirPos], getPagesPhysAddr(pageDirectory.entries[dirPos]));
    uint32_t *dirEntry = getPagesPhysAddr(pageDirectory.entries[dirPos]);
    
    sprintf("Read table at addr %d\r\n", dirEntry);

    sprintf("Current Value of dirEntry at pos: %ud\r\n", dirEntry[tabPos]);
    dirEntry[tabPos] = physAddr | PTE_USER | PTE_READ_WRITE | PTE_PRESENT;
    sprintf("New Value of dirEntry at pos: %ud\r\n", dirEntry[tabPos]);
    sprintf("Set Tab Entry to %ud - %ub\r\n", physAddr | PTE_USER | PTE_READ_WRITE | PTE_PRESENT, physAddr | PTE_USER | PTE_READ_WRITE | PTE_PRESENT);
}

// called if the pages actually fault
void pageFaultError(struct InterruptRegisters *regs) {
    pageFaultErrorBits(regs->eflags);
    kprintf("Got err code %d - %b at addr %ud\r\n", regs->eflags, regs->eflags, regs->cr2);
    kprintf("Page Fault\r\n");
    kprintf("Processor Halting\r\n");
    asm ( "hlt;\n\t" );

}

void pageFaultHandler(struct InterruptRegisters *regs) {
    // pageFaultErrorBits(0xFF);
    
    uint32_t addr = regs->cr2;
    
    sprintf("\r\n");
    sprintf("******************************\r\n");
    sprintf("*                            *\r\n");
    sprintf("*    Page Fault Handler      *\r\n");
    sprintf("*                            *\r\n");
    sprintf("******************************\r\n");
    sprintf("Addr: %d, %x\r\n", addr, addr);
    sprintf("Page Dir Index: %d, Page Table Index: %d\r\n", getPageDirIndex(addr), getPageTableIndex(addr));

    sprintf("Does Entry Exist Val: %d\r\n", doesDirEntryExist(addr));
    page_table *newPageTable = getNewPageTable();
    if (newPageTable == 0) {
        sprintf("No more page tables to hand out\r\n");
        pageFaultError(regs);
    }
    // sprintf("Got new page table\r\n");
    if (installPageDirEntry(newPageTable, getPageDirIndex(addr)) == 0) {
        sprintf("Failed to install new page dir");
        pageFaultError(regs);
    }
    sprintf("Does Entry Exist Val: %d\r\n", doesDirEntryExist(addr));
    if (doesDirEntryExist(addr) == 0) {
        sprintf("Page dir does not exist after creation\r\n");
        pageFaultError(regs);
    }
    installPageTabEntry(addr, *findNextHigherFree());
    // pageFaultError(regs);
}


void initPaging() {
    for (int i = 0; i < NUM_PAGES; i++) {
        // set each entry to have no present bit to disable it
        pageDirectory.entries[i] = PDE_USER | PDE_READ_WRITE; // | PAGING_SUPERVISOR | PAGING_WRITE; // note no present bit
    }
    
    for (int i = 0; i < NUM_PAGES; i++) {
        // set to present and r/w and at supervisor level
        pageTableOne.entries[i] = (i * 0x1000) | PTE_USER | PTE_READ_WRITE | PTE_PRESENT; // | PAGING_SUPERVISOR | PAGING_WRITE | PAGING_PRESENT;
        //                  ^
        // Mapping the addr [i]Kb to page [i] so 20-21kb is in page 20
        // so this maps from addr 0 - 1024Kb - 1MB
    
    
    }

    // put page table into page directory
    pageDirectory.entries[0] = ((unsigned int)pageTableOne.entries) | 3;// | PAGING_SUPERVISOR | PAGING_WRITE | PAGING_PRESENT;

    install_isr_handler (14, &pageFaultHandler); 

    loadPageDirectory(pageDirectory.entries);
    enablePaging();
}
