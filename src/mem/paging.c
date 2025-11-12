#include "paging.h"
#include <drivers/serial/serial.h>
#include <stdio.h>

// with help from https://wiki.osdev.org/Setting_Up_Paging and https://wiki.osdev.org/Paging and https://isu-rathnayaka.medium.com/develop-your-own-x86-operating-system-os-8-78a25b945dde

static uint32_t pageDirectory[NUM_PAGES] __attribute__((aligned(PAGE_FRAME_SIZE)));
static uint32_t pageTableOne[NUM_PAGES] __attribute__((aligned(PAGE_FRAME_SIZE)));

void initPaging() {
    for (int i = 0; i < NUM_PAGES; i++) {
        // set each entry to have no present bit to disable it
        pageDirectory[i] = 0x00000002; // | PAGING_SUPERVISOR | PAGING_WRITE; // note no present bit
    }
    
    for (int i = 0; i < NUM_PAGES; i++) {
        pageTableOne[i] = (i * 0x1000) | 3; // | PAGING_SUPERVISOR | PAGING_WRITE | PAGING_PRESENT;
    }

    // put page table into page directory
    pageDirectory[0] = ((unsigned int)pageTableOne) | 3;// | PAGING_SUPERVISOR | PAGING_WRITE | PAGING_PRESENT;

    loadPageDirectory(pageDirectory);
    enablePaging();
}
