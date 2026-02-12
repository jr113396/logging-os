#include "mallocTest.h"
#include <mem/kmalloc.h>
#include <mem/stdmem.h>
#include <stdio.h>
#include <string.h>

#include <mem/paging.h>
void mallocTest() {
    char *addr = kmalloc(1);
    // volatile char *otherAddr = kmalloc(1);

    kprintf("   got addr 0x%x", addr);
    // listPresentPages();
    do {
        kprintf("\r\n->");
        memset(addr, 0, 1024);
        getStr(addr, 1024 * 4, 1);
        sprintf("You Wrote: %s\r\n", addr);
    } while (strcmp(addr, "exit"));
    kprintf("\r\n");
    // free((uint32_t *)addr);
}