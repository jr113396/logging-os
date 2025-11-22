#include "paging.h"
#include <drivers/serial/serial.h>
#include <stdio.h>
#include <interupts/idt.h>
#include "physicalMemManager.h"

// with help from https://wiki.osdev.org/Setting_Up_Paging and https://wiki.osdev.org/Paging and https://isu-rathnayaka.medium.com/develop-your-own-x86-operating-system-os-8-78a25b945dde

static uint32_t pageDirectory[NUM_PAGES] __attribute__((aligned(PAGE_FRAME_SIZE))) = {0};
static uint32_t pageTableOne[NUM_PAGES] __attribute__((aligned(PAGE_FRAME_SIZE))) = {0}; // gives the first 4194304 bytes -> (1024 * 4) * 1024
                                                                                   //                                   ^             ^
                                                                                   //                                   |             |
                                                                                   //           Size of a page in bytes ┘          Number of pages in a table

static uint32_t pageTableTwo[NUM_PAGES] __attribute__((aligned(PAGE_FRAME_SIZE))) = {0};
page_directory *currentPageDir = 0;
physical_addr currentPageDirAddr = 0;

void treeMapPageTab(uint32_t *addr) {
    sprintf("Given Addr %ud - %ud\r\n", addr, addr);
    uint32_t *tab = addr;
    for (int i = 0; i < 1024; i++) {
        if ((tab[i] & PTE_PRESENT) != 0) {
            sprintf("\tTab Entry: %d is present\r\n", i);
        }
        else {
            sprintf("\tTab Entry: %d is not present\r\n", i);
        }
    }
}

void treeMapPageDir() {
    sprintf("\r\n");
    sprintf("******************************\r\n");
    sprintf("*                            *\r\n");
    sprintf("*    DIR Tree Map Start      *\r\n");
    sprintf("*                            *\r\n");
    sprintf("******************************\r\n");
    uint32_t *dir = currentPageDir;

    for (int i = 0; i < 1024; i++) {
        if ((dir[i] & PDE_PRESENT) != 0) {
            sprintf("Dir Entry: %d is present\r\n", i);
            // treeMapPageTab(dir[i]);
        }
        else {
            sprintf("Dir Entry: %d is not present\r\n", i);
        }
    }
}

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

void findCorrectPageLocation(uint32_t addr) {
    sprintf("\r\n");
    sprintf("******************************\r\n");
    sprintf("*                            *\r\n");
    sprintf("*    Find Page Location      *\r\n");
    sprintf("*                            *\r\n");
    sprintf("******************************\r\n");


    sprintf("Given addr %ud to find\r\n", addr);
    sprintf("Page Dir at loc: %d\r\n", pageDirectory);

    uint16_t pageDirIndex  = getPageDirIndex(addr);
    // sprintf("Page Dir Index: %d\r\n", pageDirIndex); 
    sprintf("Page Table ID: %d\r\n", pageDirIndex); 

    uint32_t *pageTable = pageDirectory[pageDirIndex];
    sprintf("Got page Table at pos %ud\r\n", pageTable);
    pageTableFlagsBits(pageTable);
    
    if (((uint32_t)pageTable & PTE_PRESENT) == 0) {
        sprintf("Page Table doesnt exist\r\n");
        sprintf("Better create it\r\n");

        pageDirectory[pageDirIndex] = ((unsigned int)pageTableTwo) | 3;
        
        pageTable = pageDirectory[pageDirIndex];
        sprintf("New values for page table after assigning\r\n");
        pageTableFlagsBits(pageTable);

    } 
    
    uint16_t pageTableIndex = getPageTableIndex(addr);
    // sprintf("Page Table Index: %d\r\n", pageTableIndex);
    sprintf("Page ID: %d\r\n", pageTableIndex);



    uint16_t page = pageTable[pageTableIndex];

    sprintf("Page Bits - %b\r\n", page);
    if ((page & PDE_PRESENT) != 0) {
        sprintf("Page Present - Not doing Anything\r\n");
    }
    else {
        sprintf("Page Not Present\r\n");
        pageTable[pageTableIndex] = (0x1000) | PTE_USER | PDE_READ_WRITE | PDE_PRESENT;
        page = pageTable[pageTableIndex];

        if ((page & PDE_PRESENT) != 0) {
            sprintf("After changing - now present\r\n");
        }
        else {
            sprintf("Still not working\r\n");
        }
    }

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

void pageDirFlagsBits(uint32_t *dir) {
    sprintf("\r\n");
    sprintf("******************************\r\n");
    sprintf("*                            *\r\n");
    sprintf("*       DIR FLAGS            *\r\n");
    sprintf("*                            *\r\n");
    sprintf("******************************\r\n");

    if ((*dir & PDE_PRESENT) != 0)
        sprintf("Dir Present\r\n");
    else
        sprintf("Dir Not Present\r\n");

    if ((*dir & PDE_READ_WRITE) != 0)
        sprintf("Dir Read/Write Access\r\n");
    else
        sprintf("Dir Read Only\r\n");

    if ((*dir & PDE_USER) != 0)
        sprintf("Dir Access By All\r\n");
    else
        sprintf("Dir SuperUser Only\r\n");

    if ((*dir & PDE_WRITE_THROUGH) != 0)
        sprintf("Dir Write Through Enabled\r\n");
    else
        sprintf("Dir Write Back Enabled\r\n");

    if ((*dir & PDE_CACHE_DISABLE) != 0)
        sprintf("Dir Page Cache Disable\r\n");
    else
        sprintf("Dir Page Cache Enabled\r\n");

    if ((*dir & PDE_ACCESSED) != 0)
        sprintf("Dir Accessed\r\n");
    else
        sprintf("Dir Not Accessed\r\n");

    if ((*dir & PDE_DIRTY) != 0)
        sprintf("Dir Dirty\r\n");
    else
        sprintf("Dir Not Dirty\r\n");

    if ((*dir & PDE_PAGE_SIZE) != 0)
        sprintf("Dir 4MB page\r\n");
    else
        sprintf("Dir 4KB page\r\n");

    if ((*dir & PDE_GLOBAL) != 0)
        sprintf("Dir Global\r\n");
    else
        sprintf("Dir Not Global\r\n");

    if ((*dir & PDE_PAT) != 0)
        sprintf("Dir PAT\r\n");
    else
        sprintf("Dir Not PAT\r\n");

    sprintf("Addr: %ud\r\n", getPagesPhysAddr(dir));
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

    sprintf("Addr: %ud\r\n", getPagesPhysAddr(tab));
    sprintf("\r\n\r\n");
}

void *allocNewPage() {
    // treeMapPageDir();

    physical_addr *addr = 0xC0000000;//findNextHigherFree();
    if (addr == 0) {
        sprintf("Error getting next free addr in getPage - got -1\r\n");
        return 0;
    }

    findCorrectPageLocation(addr);

    sprintf("Value Before %d\r\n", *addr);
    *addr = 10;
    sprintf("Value After %d\r\n", *addr);

    // page_directory *dir = currentPageDir;
    // pageDirFlagsBits(dir);

    // pd_entry *dirEntry = &dir->entries[getPageDirIndex(addr)];

    // page_table *dirTable = (page_table *)getPagesPhysAddr(dirEntry);
    // pageTableFlagsBits(dirTable);


    // pt_entry *page = &dirTable->entries[getPageTableIndex(addr)];
    // *page |= (PTE_PRESENT | PTE_READ_WRITE);
    return addr;
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
void pageFaultHandler(struct InterruptRegisters *regs) {
    // pageFaultErrorBits(0xFF);
    
    pageFaultErrorBits(regs->eflags);
    kprintf("Got err code %d - %b at addr %ud\r\n", regs->eflags, regs->eflags, regs->cr2);
    kprintf("Page Fault\r\n");
    kprintf("Processor Halting\r\n");
    asm ( "hlt;\n\t" );

}

void initPaging() {
    for (int i = 0; i < NUM_PAGES; i++) {
        // set each entry to have no present bit to disable it
        pageDirectory[i] = PDE_USER | PDE_READ_WRITE; // | PAGING_SUPERVISOR | PAGING_WRITE; // note no present bit
    }
    
    for (int i = 0; i < NUM_PAGES; i++) {
        // set to present and r/w and at supervisor level
        pageTableOne[i] = (i * 0x1000) | PTE_USER | PDE_READ_WRITE | PDE_PRESENT; // | PAGING_SUPERVISOR | PAGING_WRITE | PAGING_PRESENT;
        //                  ^
        // Mapping the addr [i]Kb to page [i] so 20-21kb is in page 20
        // so this maps from addr 0 - 1024Kb - 1MB
    
    
    }

    // put page table into page directory
    pageDirectory[0] = ((unsigned int)pageTableOne) | 3;// | PAGING_SUPERVISOR | PAGING_WRITE | PAGING_PRESENT;

    install_isr_handler (14, &pageFaultHandler); 

    loadPageDirectory(pageDirectory);
    enablePaging();
}
