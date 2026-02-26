#include "pageFault.h"
#include <stdio.h>
#include "paging.h"
#include "timers/pit/pit.h"
#include "kmalloc.h"
#include <stdlib.h>
#include "interupts/idt.h"
#include "decodeAssembly.h"
#include "mem/physicalMemManager.h"

static uint32_t *lastAddr = 0; // the last addr that was written to, captured by the page table 
static uint32_t oldData = 0; // the data that was stored in the addr before being changed 
static uint8_t newData = 0; // was there new data to read from the last addr or has it been read already ? 1 for yes, 0 for no
static uint32_t count = 0;
static logType prevType;
static uint32_t oldEIP;
static uint8_t inFault = 0;

static uint8_t nextInst[4] = {0};
static uint32_t *nextInstLoc = 0; 

static logging_data baseLog;

// with help from https://www.youtube.com/watch?v=QIGun_BGuDw


//*************************
//*                       *
//*      Logging          *
//*                       *
//*************************
// returns value of newData to other files - used to check before calling "readLastAddr()"
uint8_t isThereNewData() {
    return newData;
}

// reads and returns the last addr and then sets the newData flag to 0 as it has now been read
uint32_t readLastAddr() {
    uint32_t data = *(uint32_t *)lastAddr;

    if (prevType == WRITE) {
        kprintf("[LOGGING] Addr %x changed from %ud to %ud\r\n", lastAddr, oldData, data);
        sprintf("[LOGGING] Addr %x changed from %ud to %ud\r\n", lastAddr, oldData, data);
    }
    if (prevType == READ){
        kprintf("[LOGGING] Addr %x read - had value %ud\r\n", lastAddr, oldData);
        sprintf("[LOGGING] Addr %x read - had value %ud\r\n", lastAddr, oldData);
    }
    newData = 0; // set new data to 0 as we have just read the new data;
    return data;
}
logging_data *createLoggingEvent() {
    uint32_t currentData = *(uint32_t *)lastAddr;
    sprintf("Allocating mem for logging event\r\n");
    logging_data *data = &baseLog;//kmalloc(sizeof(logging_data));
    data->addr = (uint32_t)lastAddr;
    data->before = oldData;
    data->after = currentData;
    data->numTicks = getTickCount();
    data->type = prevType;
    data->next = NULL;
    newData = 0;
    return data;
}
uint32_t getOldData() {
    return oldData;
}

logType getLogType() {
    return prevType;
}

//*************************
//*                       *  
//*    Error Debugging    *
//*                       *
//*************************
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
        sprintf("Software Guard Extensions \t -> Fault due SGX violation -> not related to normal paging \r\n");

    sprintf("\r\n\r\n");
}


//*************************
//*                       *
//*    Fault Handler      *
//*                       *
//*************************

// called if the pages actually fault
void pageFaultError(struct InterruptRegisters *regs) {
    sprintf("[PAGEFAULTERROR]\r\n");
    pageFaultErrorBits(regs->eflags);
    sprintf("Got err code %d - %b at addr %ud\r\n", regs->eflags, regs->eflags, regs->cr2);
    
    kprintf("Got err code %d - %b at addr %ud\r\n", regs->eflags, regs->eflags, regs->cr2);
    kprintf("Page Fault\r\n");
    kprintf("Processor Halting\r\n");
    asm ( "hlt;\n\t" );

}


// this function is called when the page fault is caused by the logging system not an actual page fault otherwise the pageFaultError function is called
void loggingPagingHandler(struct InterruptRegisters *regs) {
    sprintf("[ENTERING LOGGING HANDLER]\r\n");
    // sprintf("Dealing with page table logging\r\n"); 
    page_directory pageDirectory = getPageDir();
    uint32_t addr = regs->cr2;
    uint8_t isWrite = ((regs->errorCode & (1 << 1)) != 0);
    uint32_t pageDirIndex = getPageDirIndex(addr);
    sprintf("Addr: %x\r\n", regs->cr2);
    // pageFaultErrorBits(regs->errorCode);
    // sprintf("setting page dir ent to present - was %d\r\n", pageDirectory.entries[pageDirIndex] & PDE_PRESENT);
    // pageDirectory.entries[pageDirIndex] |= PDE_PRESENT; // enable the page dir

    setDirEntPres(pageDirIndex);

    // sprintf("Dir is now %d\r\n", pageDirectory.entries[pageDirIndex] & PDE_PRESENT);

    uint32_t *pageDirPhysVal = getPagesPhysAddr(pageDirectory.entries[pageDirIndex]);
    page_table *pageTable = (page_table *)pageDirPhysVal; // get the page table from the page dir
    
    uint32_t pageTableIndex = getPageTableIndex(addr); 

    // check the addr of the page table and page table entry to make sure they are actually the correct values
    if (pageDirPhysVal == 0 || getPagesPhysAddr(pageTable->entries[pageTableIndex]) == 0) {
        // the page table entry does not exist?
        sprintf("Page Dir Entry %d\r\n", pageDirIndex);
        sprintf("Page table entry %d does not exist\r\n", pageTableIndex);
        pageFaultError(regs);

        asm("hlt"::);
    }
    // sprintf("Setting page table to present - was %d\r\n", pageTable->entries[pageTableIndex] & PTE_PRESENT);
    pageTable->entries[pageTableIndex] |= PTE_PRESENT; // enable the page table

    // has the data been read since the last go round?
    if (isThereNewData()) {
        // no
        // so read it
        readLastAddr();
    }
    if (inFault <= 1) {
        // set the addr that is attempted to be written to
        lastAddr = (uint32_t *)regs->cr2;
    
        // set old data to be the data that was there before
        oldData = *lastAddr;
    
        // and set the flag that there is new data
        newData = 1;
    
        if ((regs->errorCode & (1 << 1)) != 0)
            // sprintf("Write Access\r\n");
            prevType = WRITE;
        else
            // sprintf("Read Access\r\n");
            prevType = READ;

    }
    nextInstLoc = (uint8_t *)regs->eip;
    uint8_t nextInst = *nextInstLoc;
    viewMem(nextInstLoc-3, 6);

    *nextInstLoc = 0xCC; // debugger call to int 3 https://c9x.me/x86/html/file_module_x86_id_142.html
    kprintf("EIP1: %d - %x\r\n", nextInstLoc, nextInst);
    viewMem(nextInstLoc-3, 6);
    // asm("hlt"::);
    count++;
    sprintf("[LEAVING LOGGING HANDLER]\r\n");

}

void breakpointHandler(struct InterruptRegisters *regs) {
    sprintf("[ENTERING BREAKPOINT HANDLER]\r\n");
    // kprintf("Break point handler running\r\n");
    // displayRegisters(regs);
    uint8_t newEIP = *((uint32_t *)regs->eip);
    // sprintf("NewEIP: %x\r\n", newEIP);
    // sprintf("NextLoc: %x - EIP: %x - %x\r\n", nextInstLoc, regs->eip, *(uint8_t *)regs->eip);

    *((uint8_t*)regs->eip) = nextInst;
    viewMem(nextInstLoc-3, 6);

    // asm("hlt;": :);
}

void pageFaultHandler(struct InterruptRegisters *regs) {
    sprintf("[ENTERING PAGEFAULT HANDLER]\r\n");
    setAutoClearPages(0);
    inFault++;
    
    // sprintf("\r\n");
    // sprintf("******************************\r\n");
    // sprintf("*                            *\r\n");
    // sprintf("*    Page Fault Handler      *\r\n");
    // sprintf("*                            *\r\n");
    // sprintf("******************************\r\n");
    
    // sprintf("Disabling autoclear pages\r\n");
    uint32_t addr = regs->cr2;
    page_directory pageDirectory = getPageDir();

    // sprintf("Addr: %d, %x\r\n", addr, addr);
    // sprintf("Page Dir Index: %d, Page Table Index: %d\r\n", getPageDirIndex(addr), getPageTableIndex(addr));
    
    // addr - the addr being written to

    uint32_t pageDirIndex = getPageDirIndex(addr),
             pageTabIndex = getPageTableIndex(addr);

    
    // at this point the dir is not present
    // so we need to work out if it ever existed or was it disabled by the logging system?
    
    // get the avl bits of the page dir to see if it is disabled or not
    uint8_t avlBits = ((pageDirectory.entries[pageDirIndex] >> avlBitPos) & 0b111);
    // sprintf("AvlBits for tab %d: %b - %d\r\n", pageDirIndex, avlBits, avlBits);

    // #ifdef pagingLogging
    // sprintf("%b - %b \r\n", (avlBits == 0b110), pageDirectory.entries[pageDirIndex]) ;
    if ((avlBits & 0b111) == 0b110) {
        // sprintf("running logging handler\r\n");
        // this page fault was caused by the logging system rather than 
        // by an actual page fault

        loggingPagingHandler(regs);
        // sprintf("Dir is now %d\r\n", pageDirectory.entries[pageDirIndex] & PDE_PRESENT);

       
    } else {
    // #endif

        // this page fault was actually caused by a page fault rather than by 
        // Make page table
        #ifdef autofillpagetable
            kprintf("INSTALLING NEW PAGE TABLE\r\n");

            page_table *newPageTable = (page_table *)getNewPageTable();
            if (newPageTable == 0) {
                sprintf("No more page tables to hand out\r\n");
                pageFaultError(regs);
            }
            sprintf("AF1\r\n");
            // sprintf("Got new page table\r\n");
            if (installPageDirEntry(newPageTable, getPageDirIndex(addr)) == 0) {
                sprintf("Failed to install new page dir");
                pageFaultError(regs);
            }
            sprintf("AF2\r\n");

            sprintf("Does Entry Exist Val: %d\r\n", doesDirEntryExist(addr));
            if (doesDirEntryExist(addr) == 0) {
                sprintf("Page dir does not exist after creation\r\n");
                // pageFaultError(regs);
            }

            sprintf("AF3\r\n");
            installPageTabEntry(addr, (uint32_t)findNextHigherFree(1));
            sprintf("AF4\r\n");

            kprintf("INSTALLED NEW PAGE TABLE");
            // loggingPagingHandler(regs);
        
        #else
            // we are not automatically filling the page table so cause faults.   
            kprintf("Not autofilling page table\r\n"); 
            pageFaultError(regs);

        #endif
    // #ifdef pagingLogging
    }
    // #endif

    // sprintf("PageDir Present: %d\r\n", pageDirectory.entries[getPageDirIndex(addr)] & PDE_PRESENT); 

    // pageDirectory.entries[getPageDirIndex(addr)] |= PDE_PRESENT;
    // page_table *pageTable = (page_table *)pageDirectory.entries[getPageDirIndex(addr)];

    // sprintf("PageTable Present: %d\r\n", pageTable->entries[getPageTableIndex(addr)] & PDE_PRESENT);
    // pageTable->entries[getPageTableIndex(addr)] |= PDE_PRESENT;
    // sprintf("Set table to present\r\n");

    // pageDirFlagsBits(addr);

    // get the avl bits of the page activated if they are 110 then this is a page that has been disabled but should be valid
    // so set it to useable - remember to set back to 110 when disabling the pages 

    #ifdef pagefaultlisting
        kprintf("Register\tAddr \tValue\r\n");
        kprintf("CR2 Val:\t--\t\t \t%ud \t%ud\r\n",                   (uint32_t *)(regs->cr2),     0);
        kprintf("DS Val: \t%d\t \t%ud \t%ud\r\n",  (regs->ds),     *(uint32_t *)(regs->ds),      (*(uint32_t **)(regs->ds)));
        kprintf("edi Val:\t%d\t \t%ud \t%ud\r\n", (regs->edi),     *(uint32_t *)(regs->edi),     (*(uint32_t **)(regs->edi)));
        kprintf("esi Val:\t%d\t \t%ud \t%ud\r\n", (regs->esi),     *(uint32_t *)(regs->esi),     (*(uint32_t **)(regs->esi)));
        kprintf("ebp Val:\t%d\t \t%ud \t%ud\r\n", (regs->ebp),     *(uint32_t *)(regs->ebp),     (*(uint32_t **)(regs->ebp)));
        kprintf("esp Val:\t%d\t \t%ud \t%ud\r\n", (regs->esp),     *(uint32_t *)(regs->esp),     (*(uint32_t **)(regs->esp)));
        kprintf("ebx Val:\t%d\t \t%ud \t%ud\r\n", (regs->ebx),     *(uint32_t *)(regs->ebx),     (*(uint32_t **)(regs->ebx)));
        kprintf("edx Val:\t%d\t \t%ud \t%ud\r\n", (regs->edx),     *(uint32_t *)(regs->edx),     (*(uint32_t **)(regs->edx)));
        kprintf("ecx Val:\t%d\t \t%ud \t%ud\r\n", (regs->ecx),     *(uint32_t *)(regs->ecx),     (*(uint32_t **)(regs->ecx)));
        kprintf("eax Val:\t--\t \t%ud \t%ud\r\n",                   (uint32_t *)(regs->eax),     0); //(*(uint32_t **)(regs->eax))); // also causes a page fault
        kprintf("eip Val:\t%d\t \t%ud \t%ud\r\n", (regs->eip),     *(uint32_t *)(regs->eip),     (*(uint32_t **)(regs->eip)));
        kprintf("cms Val:\t%d\t \t%ud \t%ud\r\n", (regs->csm),     *(uint32_t *)(regs->csm),     (*(uint32_t **)(regs->csm)));
        kprintf("efg Val:\t%d\t \t%ud \t%ud\r\n", (regs->eflags),  *(uint32_t *)(regs->eflags),  0);
        kprintf("usp Val:\t%d\t \t%ud \t%ud\r\n", (regs->useresp), *(uint32_t *)(regs->useresp), 0); 
        kprintf("ss  Val:\t%d\t \t%ud \t%ud\r\n", (regs->ss),      *(uint32_t *)(regs->ss),      0);
        kprintf("page handler: %d\r\n", &pageFaultHandler);
       
        asm("hlt;"::);
    #endif
 
    /* This is to do with attempting to set the next bit to be an invalid opcode so that it opcode faults on the next instruction */
    // kprintf("eip Val:\t%d\t \t%ud \t%ud\r\n", (regs->eip),     *(uint32_t *)(regs->eip),     (*(uint32_t **)(regs->eip)));
    //oldData = *(uint32_t *)(regs->eip);
    // *(uint32_t *)(regs->eip) = 0;// 0x0F0E;
    // sprintf("Enabling autoclear pages\r\n");
    // if (count < 10) 
    inFault--;
    setAutoClearPages(1);
    if (count > 10000) {
        kprintf("Max PageFaults - Halting\r\n");
        asm("hlt;"::);
    }
    sprintf("[PAGEFAULT HANDLER RETURNING]\r\n");
}

