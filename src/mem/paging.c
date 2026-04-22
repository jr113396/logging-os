#include "paging.h"
#include <drivers/serial/serial.h>
#include <stdio.h>
#include <interupts/idt.h>
#include "physicalMemManager.h"
#include "pageFault.h"
#include "invalidOpcodeFault.h"
#include "loggingLevel.h"
// with help from https://wiki.osdev.org/Setting_Up_Paging and https://wiki.osdev.org/Paging and https://isu-rathnayaka.medium.com/develop-your-own-x86-operating-system-os-8-78a25b945dde

// static uint32_t pageDirectory[NUM_PAGES] __attribute__((aligned(PAGE_FRAME_SIZE))) = {0};
// static uint32_t pageTableOne[NUM_PAGES] __attribute__((aligned(PAGE_FRAME_SIZE))) = {0}; // gives the first 4194304 bytes -> (1024 * 4) * 1024
//                                                                                          //                                   ^             ^
                                                                                            //                                   |             |
                                                                                            //           Size of a page in bytes ┘          Number of pages in a table

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

page_directory pageDirectory;// __attribute__((aligned(PAGE_FRAME_SIZE)));
page_table pageTableOne;// __attribute__((aligned(PAGE_FRAME_SIZE)));  

int getPageDirIndex(uint32_t addr) {
    return addr >> 22; // get what page entry contains this addr 0-1023
}

int getPageTableIndex(uint32_t addr) {
    return ((addr >> 12) & 1023); // get what page frame contains this addr 0-1023 - removing the flags
}

uint32_t *getPagesPhysAddr(uint32_t val) {
    return (uint32_t *)(val & ~0xFFF); // remove the flags from the entry and return 
}

page_directory getPageDir() {
    return pageDirectory;
} 

void setDirEntPres(uint32_t dirIndex) {
    // sprintf("Enabling PDE %d\r\n", dirIndex);
    pageDirectory.entries[dirIndex] |= PDE_PRESENT;
}

int doesDirEntryExist(uint32_t addr) {
    uint16_t index = getPageDirIndex(addr);
    uint32_t pageDirEntry = pageDirectory.entries[index];
    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
        sprintf("[doesDirEntryExist] Found index for addr: %ud\r\n", addr); 
        sprintf("[doesDirEntryExist] entry has value %d - is Present %d\r\n", pageDirEntry, pageDirEntry & PDE_PRESENT);
    #endif
    return pageDirEntry & PDE_PRESENT;
}



uint32_t *getNewPageTable() {
    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
    sprintf("Getting pages from higher mem\r\n");
    #endif
    uint32_t *addr = findNextHigherFree(1);
    if (addr == 0) {
        #if _SERIAL_LOGGING_LEVEL >= _LOGGING_ERROR
        sprintf("Returned addr == 0, should prob panic AAAAA!\r\n");
        #endif
        return 0;
    }
    return addr; // return the address for it to be used 
    // return pageTable2;
    // return 0;
    // return &pageTableTwo;
}

int installPageDirEntry(page_table *table, uint16_t pos) {
    if (pos > 1024) {
        #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
        sprintf("Install page Dir got handed pos greater than 1024\r\n");
        #endif
        return 0;
    }
    // install a page directory entry at a position in the page dir entry
    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
    sprintf("Pos: %d\r\n", pos);
    sprintf("Got table value: %ud\r\n", (table->entries));
    #endif
    #if 1
        pageDirectory.entries[pos] = ((uint32_t)(table->entries)) | PDE_USER | PDE_READ_WRITE | PDE_PRESENT | (0b110 << avlBitPos); // set the corresponding avl bits but dont set it as present as we want page faults
        // sprintf("Installed page dir entry with avl bits 0b110\r\n");
    #else
        pageDirectory.entries[pos] = ((uint32_t)(table->entries)) | PDE_USER | PDE_READ_WRITE | PDE_PRESENT;
    #endif
    return 1;
}

void listPresentPages() {

    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_INFO
    sprintf("*****   LISTING PRESENT PAGES ****\r\n");
    #endif
    for (int PDE_NUM = 0; PDE_NUM < NUM_PAGES; PDE_NUM++) {
        if (pageDirectory.entries[PDE_NUM] & PDE_PRESENT) {
            #if _SERIAL_LOGGING_LEVEL >= _LOGGING_INFO
            sprintf("PDE: %d -> PRESENT\r\n", PDE_NUM);
            sprintf("PDE Phys Addr: %x\r\n", pageDirectory.entries[PDE_NUM]);
            #endif
            uint32_t pageDirEntryVal = pageDirectory.entries[PDE_NUM];
            uint32_t *pageDirPhysVal = getPagesPhysAddr(pageDirEntryVal);
            page_table *pt = (page_table *) *pageDirPhysVal;
            char lastWasPres = 0;
            uint16_t lastTimePres = 0;
            for (int PTE_NUM = 0; PTE_NUM < NUM_PAGES; PTE_NUM++) {
                char isPTEPres = pt->entries[PTE_NUM] & PTE_NUM;
                if (!lastWasPres && isPTEPres) 
                    lastTimePres = PTE_NUM;
                #if _SERIAL_LOGGING_LEVEL >= _LOGGING_INFO
                sprintf("PTE_NUM: %d - %d\r\n",&pt->entries[PTE_NUM], PTE_NUM);
                #endif
                if (lastWasPres & !isPTEPres) {
                    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_INFO
                    sprintf("\tPTE %d to %d pres\r\n", lastTimePres, PTE_NUM-1);
                    #endif
                }
                lastWasPres = isPTEPres;
            }
        }
    }
}

void installPageTabEntry(uint32_t virtAddr, uint32_t physAddr) {
    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
    sprintf("[ENTERING INSTALLPAGETABENT]\r\n");
    #endif
    // virtAddr - where the program thinks its trying to access
    // physAddr - where the data is actually going to be stored
    uint16_t dirPos = getPageDirIndex(virtAddr),
             tabPos = getPageTableIndex(virtAddr);

    uint32_t flags = (uint32_t)getPagesPhysAddr(physAddr) | PTE_USER | PTE_READ_WRITE | PTE_PRESENT | (0b110 << avlBitPos);
    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
    sprintf("Dir Pos: %d\r\n", dirPos);
    sprintf("Tab Pos: %d\r\n", tabPos);
    #endif

    if (doesDirEntryExist(virtAddr) == 0) {
        #if _SERIAL_LOGGING_LEVEL >= _LOGGING_ERROR
        sprintf("Page dir does not exist after creation\r\n");
        #endif
        // pageFaultError(regs);
    }
    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
    sprintf("Page Dir Entry Val: %d - addr %d\r\n", pageDirectory.entries[dirPos], getPagesPhysAddr(pageDirectory.entries[dirPos]));
    #endif
    page_table *pt = (page_table *)getPagesPhysAddr(pageDirectory.entries[dirPos]);
    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
    sprintf("Read table at addr %d\r\n", pt);
    #endif
    // sprintf("Current Value of pt at pos: %ud\r\n", pt->entries[tabPos]);
    pt->entries[tabPos] = flags;
    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
    sprintf("New Value of pt at pos: %ud\r\n", pt->entries[tabPos]);
    sprintf("Set Tab Entry to %ud - %b\r\n", flags, flags);
    sprintf("[LEAVING INSTALLPAGETABENT]\r\n");
    #endif

}


void disablePages() {
    page_table *pg1 = (page_table *)pageDirectory.entries[0];  

    // for (int i = 800; i < NUM_PAGES; i++) {
    //     // disable the top pages of page table 1
    //     pg1->entries[i] &= ~(PDE_PRESENT); // remove present bit
    //     pg1->entries[i] &= ~(0b111 << 9); // clear old avl bits
    //     pg1->entries[i] |= (0b111 << 9); // set new avl bits
    // }

    // disable all pages that arent to do with the kernel so ones from kmalloc
    for (int i = 1; i < NUM_PAGES; i++) {
       if (pageDirectory.entries[i] & (PDE_PRESENT)) {
            // sprintf("****************8\r\n");
            // sprintf("Page dir %d was present - changing\r\n", i);
            pageDirectory.entries[i] &= ~(PDE_PRESENT); // clear the present bit
            // and then add that it has been modified by this function by setting the avl bits to 110
            pageDirectory.entries[i] &= ~(0b111 << avlBitPos); // clear the avl bits
            pageDirectory.entries[i] |= (0b110 << avlBitPos); // and set them to the correct values
            // sprintf("Page dir %d now has pres val %d\r\n", i, pageDirectory.entries[i] & (PDE_PRESENT)); 
        }
    }
    loadPageDirectory(pageDirectory.entries);

}

void installNewPageFull(uint32_t addr, uint32_t physAddr) {
    // so we have a page directory
    // first get the page directory and table this addr should be in
    uint32_t dirIndex = getPageDirIndex(addr),
             tabIndex = getPageTableIndex(addr);



    kprintf("INSTALLING NEW PAGE\r\n");
    
    // does the page dir entry exist?
    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
    sprintf("Installing New Page at dir: %d - tab: %d\r\n", dirIndex, tabIndex);
    #endif
    
    // PAGE DIR
    // does the page dir entry exist / is present
    uint32_t pageDirEntryVal = pageDirectory.entries[dirIndex];
    uint32_t pageDirEntIsPresent = pageDirEntryVal & PDE_PRESENT;
    uint32_t *pageDirPhysVal = getPagesPhysAddr(pageDirEntryVal);

    if (pageDirPhysVal) { // not 0 
        // then there is a page table already installed in this location
        if (pageDirEntIsPresent) {
            // the table is present so we are all good yay 
            kprintf("PDE is already present\r\n");
        }
        else {
            // the table is not present but is installed
            // so lets make it present with the correct addr
            pageDirectory.entries[dirIndex] |= PDE_PRESENT;
            // now that PDE should be valid
            kprintf("Made the PDE present\r\n");
        }
    } else {
        // otherwise there is no page dir entry linking to a page table
        // so we need to make a new page table to link to this page table entry
        kprintf("No PDE - Dealing with later\r\n");
        return;
    }
    
    // now we are going to re get the PDE in case it has changed
    pageDirEntryVal = pageDirectory.entries[dirIndex]; 
    // and get the phys addr corresponding to it - this will be the addr of the page table
    pageDirPhysVal = getPagesPhysAddr(pageDirEntryVal);

    kprintf("Dir Ind: %d,  Tab Ind: %d\r\n", dirIndex, tabIndex);
    kprintf("Page Dir Ent Val:      %d\r\n", pageDirEntryVal);
    kprintf("Page Dir Phy Val:      %d\r\n", pageDirPhysVal);
    kprintf("Page Dir Ent Is Pres:  %d\r\n", pageDirEntIsPresent);
    kprintf("=====================\r\n");
    
    // get the page table from the phys addr provided by the PTE
    page_table *pt = (page_table *) *pageDirPhysVal;
    
    // now we are dealing with the page table 
    // so we need to check to see if the page table entry for this already exists or not
    uint32_t pageTabEntryVal = pt->entries[tabIndex];
    uint32_t *pageTabPhysVal = getPagesPhysAddr(pageTabEntryVal);
    uint32_t pageTabIsPres = pageTabEntryVal & PTE_PRESENT;
    
    kprintf("Page Tab Ent Val: %b\r\n", pageTabEntryVal);
    kprintf("Page Tab Phy Val: %d\r\n", pageTabPhysVal);
    kprintf("Page Tab Is Pres: %d\r\n", pageTabIsPres);
    if (pageTabIsPres) {
        kprintf("PTE is already present\r\n");
        // if the page table is already present
        // then dont do anything as its already setup

    } else {
        kprintf("SET PTE to Present and addr\r\n");
        // make the page table present
        // and make it have the correct addr
        pt->entries[tabIndex] = physAddr | PTE_USER | PTE_READ_WRITE | PTE_PRESENT;
        kprintf("New Val: %b\r\n", pt->entries[tabIndex]);
    }
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

    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
    sprintf("[PAGINGINIT] PD %d - PT1 %d\r\n", pageDirectory, pageTableOne);
    #endif

    install_isr_handler (14, &pageFaultHandler); 
    install_isr_handler (6, &invalidOpcodeHandler);
    install_isr_handler (3, &breakpointHandler);

    loadPageDirectory(pageDirectory.entries);
    enablePaging();
    #ifdef pagingLogging
        #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
        sprintf("Disabling Paging\r\n");
        #endif
        disablePages(); // disable page for the logging system
    #endif
}
