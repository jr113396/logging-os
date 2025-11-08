#include "paging.h"
#include <drivers/serial/serial.h>
#include <stdio.h>

// with help from https://wiki.osdev.org/Setting_Up_Paging and https://wiki.osdev.org/Paging
static uint32_t pageDirectory[1024] __attribute__((aligned(4096)));
static uint32_t pageTableOne[1024] __attribute__((aligned(4096)));

void initPaging() {
    for (int i = 0; i < 1024; i++) {
        // set each entry to have no present buit ot disable it
        pageDirectory[i] = 0x00000002; // | PAGING_SUPERVISOR | PAGING_WRITE; // note no present bit
    }
    
    for (int i = 0; i < 1024; i++) {
        pageTableOne[i] = (i * 0x1000) | 3; // | PAGING_SUPERVISOR | PAGING_WRITE | PAGING_PRESENT;
    }

    // put page table into page directory
    pageDirectory[0] = ((unsigned int)pageTableOne) | 3;// | PAGING_SUPERVISOR | PAGING_WRITE | PAGING_PRESENT;

    loadPageDirectory(pageDirectory);
    enablePaging();
}

void *getPhysicalAddr(void *virtualAddr) {
    uint32_t dirIndex = (uint32_t)virtualAddr >> 22; // index of the vaddr in the paging directory 
    uint32_t tabIndex = (uint32_t)virtualAddr >> 12 & 0x03FF; // index of the vaddr in the paging table

    if (*pageDirectory & ((uint32_t)1) == 1) {
        uint32_t *pageTable = pageDirectory + (0x400 + dirIndex);// NOTE - This might cause issues - osdev uses "((unsigned long *)0xFFC00000) + (0x400 * pdindex);" for some reason
        // page directory is present
        // check if the table is present
        if (*pageTable & ((uint32_t)1) == 1) { 
            #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                sprintf("[DEBUG][PAGING] Found page table %d and dir %d when looking for addr %d\r\n", tabIndex, dirIndex, virtualAddr);
            #endif

            return (void *)((pageTable[tabIndex] & ~0xFFF) + (uint32_t)virtualAddr & 0xFFF);
        }
        else {
            #if _SERIAL_LOGGING_LEVEL >= _LOGGING_ERROR
                sprintf("[ERROR] [PAGING] Page table %d not present in dir %d when looking for addr %d\r\n", tabIndex, dirIndex, virtualAddr);
            #endif
            #if _PRINT_LOGGING_LEVEL >= _LOGGING_ERROR
                kprintf("[ERROR] [PAGING] Page table %d not present in dir %d when looking for addr %d\r\n", tabIndex, dirIndex, virtualAddr);
            #endif
            return 0;
        }
    }
    else {
        #if _SERIAL_LOGGING_LEVEL >= _LOGGING_ERROR
            sprintf("[ERROR] [PAGING] Page directory %d not present when looking for addr %d\r\n", dirIndex, virtualAddr);
        #endif
        #if _PRINT_LOGGING_LEVEL >= _LOGGING_ERROR
            kprintf("[ERROR] [PAGING] Page directory %d not present when looking for addr %d\r\n", dirIndex, virtualAddr);
        #endif
        return 0;
    }
}