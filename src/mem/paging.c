#include "paging.h"

// with help from https://wiki.osdev.org/Setting_Up_Paging
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