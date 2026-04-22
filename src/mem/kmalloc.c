#include "stdmem.h"
#include "physicalMemManager.h"
#include "paging.h"
#include "kmalloc.h"
#include <maths.h>
#include <stdio.h>
#include <io/io.h>
#include "timers/pit/pit.h"
#include <loggingLevel.h>
uint32_t *kmallocLocations[KMALLOC_SPLITS] = {0}; // an array of pointers to memory locations where smaller values (smaller than a page) are stored 
uint32_t *kmallocMaps[KMALLOC_SPLITS] = {0};

int kallocInit() {
    for (int i = 0; i < KMALLOC_SPLITS; i++) {
        kmallocLocations[i] = findNextHigherFree(1); // get a page to split up and assign out
        if (kmallocLocations[i] == 0) {
            #if _SERIAL_LOGGING_LEVEL >= _LOGGING_ERROR
            sprintf("Failed to get a page for index %d in kmallocLocations\r\n", i);
            #endif
            return 0;
        }
        kmallocMaps[i] = findNextHigherFree(1); // get a page to split up to be a bitmap of where is allocated in the other page
        if (kmallocMaps[i] == 0) {
            #if _SERIAL_LOGGING_LEVEL >= _LOGGING_ERROR
            sprintf("Failed to get a page for index %d in kmallocMaps\r\n", i);
            #endif
            return 0;
        }

        memset(kmallocMaps[i], 0, PAGE_SIZE); // set all the values to be 0 in the bitmap
    }
    return 1;
}

uint32_t *findNextFreeSmallMap(uint16_t numBytes) {
    int index = 0;
    const int numIndex = 12 - KMALLOC_SPLITS + 1; // 12 from log2(PAGE_SIZE) - dont have log implemented yet
    for (int i = 0; i < KMALLOC_SPLITS; i++) {
        #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
        sprintf("Checking shift %d - val: %d\r\n", (numIndex+i), 1 << (numIndex+i));
        #endif
        if (numBytes < (1 << (numIndex + i))) {
            #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
            sprintf("Found with shift %d  - val: %d\r\n", (numIndex+i), 1 << (numIndex+i));
            #endif
            index = i;
            break;
        }
    }
    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
    sprintf("Got Index: %d from bytes: %d\r\n", index, numBytes);
    sprintf("Number of entries: %d - entry size = %d bytes\r\n", PAGE_SIZE/(1 << (index+numIndex)), (1<<(index+numIndex)));
    #endif
    for (int i = 0; i < PAGE_SIZE/(1 << (index+numIndex)); i++) {
        #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
        sprintf("Checking Large: %d, Small %d\r\n", i/32, mod(i,32));
        #endif
        uint32_t loc = kmallocMaps[index][i/32] & (1 << mod(i, 32));
        #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
        sprintf("Loc: %d\r\n", loc);
        #endif
        if (loc == 0) {
            #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
            sprintf("Found a free space\r\n");
            #endif
            kmallocMaps[index][i/32] |= (1 << mod(i,32)); // set that bit to be 1 as found
            return kmallocLocations[index] + (1 << (index+numIndex)) * i; // get the start of the chunk of that size blocks + how far into that chunk you found a free space
        }
    }
    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_ERROR
    sprintf("No space found\r\n");
    #endif
    return 0; // did not find a space
    // seperate locations for storing 
    // 128 = 
    //  001 - 128  bytes
    //  129 - 256  bytes
    //  257 - 512  bytes
    //  513 - 1024 bytes
    // 1025 - 2048 bytes
    // 2049 - 4095 bytes
    // 4096+ - allocate n pages

}
void  *kmalloc(uint32_t numBytes) {
    setAutoClearPages(0);

    if (numBytes == 0)
        return 0;
    uint32_t *addr = 0;
    // seperate locations for storing 
    // LOWER (BYTES) | UPPER (BYTES) | AVL BITS
    // -----------------------------  
    //       001     |      128      | 000
    //       129     |      256      | 001
    //       257     |      512      | 010
    //       513     |     1024      | 011
    //      1025     |     2048      | 100
    //      2049     |     4095      | 101
    //      4096     |     4096      | 110
    //      4097+    | alloc n pages | 111


    // *************************
    // turn this on when you work out how to free less than a page
    // ************************
    // if (numBytes < PAGE_SIZE) {
    //     // less than a page size so should be split up
    //     addr = findNextFreeSmallMap(numBytes);
    // }
    if (addr == 0) {
        #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
        sprintf("Allocating pages as more than %d bytes given - %d\r\n", PAGE_SIZE, numBytes);
        #endif
        int numPages;
        if (mod(numBytes, PAGE_SIZE) == 0) 
            numPages = numBytes/PAGE_SIZE;
        else
            numPages = (numBytes / PAGE_SIZE) + 1; // need to have the +1 for all the little left over bits

        #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
        sprintf("[kmalloc] Allocating %d pages for %d bytes\r\n", numPages, numBytes);
        #endif
        addr = findNextHigherFree(numPages);
    }

    // now that we have an addr
    // install it into the page dir
    #pragma GCC diagnostic ignored "-Wunused-but-set-variable"
    page_table *PT;
    page_directory pageDirectory = getPageDir();
    uint32_t PD_Index = getPageDirIndex((uint32_t)addr);
    uint32_t PT_Index = getPageTableIndex((uint32_t) addr);
    if (pageDirectory.entries[PD_Index] & PDE_PRESENT) {
        uint32_t *pageDirPhysVal = getPagesPhysAddr(pageDirectory.entries[PD_Index]);
        #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
            sprintf("[KMALLOC] Already had present page dir\r\n");
            sprintf("[KMALLOC] Page Dir Phys Val: %x\r\n", pageDirPhysVal);
        #endif
        PT = (page_table *) *pageDirPhysVal;
        // if (pt.entries[])
    }
    else {
        #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
        sprintf("[KMALLOC] No PT so making a new one \r\n");
        #endif
        page_table *PT = (page_table *)findNextLowerFree(1);
        if (PT == 0) {
            #if _SERIAL_LOGGING_LEVEL >= _LOGGING_ERROR
            sprintf("[KMALLOC] Couldnt malloc page table\r\n");
            #endif
            // return 0 as error
            return 0;
        }
        if (installPageDirEntry(PT,PD_Index) == 0) {
            #if _SERIAL_LOGGING_LEVEL >= _LOGGING_ERROR
            sprintf("[KMALLOC] Failed to install new page table\r\n");
            #endif
            return 0;
        }
        
    }
    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
    sprintf("Trying to install page tab entry at %d %d\r\n", PD_Index, PT_Index);
    #endif
    installPageTabEntry((uint32_t)addr, (uint32_t)addr);
    
    setAutoClearPages(1);
    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
    sprintf("Installed new page tab at addr: %d\r\n", addr);
    #endif
    disablePages();
    return addr;
}

uint32_t free(uint32_t *addr) {
    // printf 
    return returnHigherPage(addr);
}

