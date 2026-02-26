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
    kprintf("\r\nRunning mallocTest\r\n");
    char *addr = kmalloc(1);
    // volatile char *otherAddr = kmalloc(1);

    kprintf("   got addr 0x%x\r\n", addr);
    kprintf("STRCMP: %d\r\n", strcmp("abc", "abc"));
    // listPresentPages();
    do {
        kprintf("\r\n->");
        memset(addr, 0, 1024);
        getStr(addr, 1024, 1);
        sprintf("You Wrote: %s\r\n", addr);
        sprintf("\r\n***\r\n\r\n");
        sprintf("STRLEN: %d\r\n", strlen(addr));

        // if (strcmp(addr, "count") == 0) {
        //     loop((uint32_t *)addr, 1000);
        // }
        sprintf("\r\n****\r\n\r\n");

        sprintf("Running into the while\r\n");
    } while (sprintf("Inwhile %s\r\n", addr),strcmp(addr, "exit"), sprintf("End of while\r\n"), 1);
    kprintf("\r\n");
    // free((uint32_t *)addr);
}