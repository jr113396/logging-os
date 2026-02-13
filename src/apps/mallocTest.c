#include "mallocTest.h"
#include <mem/kmalloc.h>
#include <mem/stdmem.h>
#include <stdio.h>
#include <string.h>
#include <timers/pit/pit.h>
#include <mem/paging.h>
#include <maths.h>
#include "apps/tick-monitor.h"

void loop(volatile uint32_t *start, uint32_t count) {
    kprintf("Running Count\r\n");
    *(uint32_t *)start = 0;
    outNumTicks();
    for (uint32_t i = 0; i < count; i++) {
        *(start + mod(i, 4) * sizeof(uint32_t)) = i;
        pauseTicks(1);
    }
    outNumTicks();
}


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

        if (strcmp(addr, "count") == 0) {
            loop((uint32_t *)addr, 1000);
        }
    } while (strcmp(addr, "exit"));
    kprintf("\r\n");
    // free((uint32_t *)addr);
}