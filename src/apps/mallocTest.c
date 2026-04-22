#include "mallocTest.h"
#include <mem/kmalloc.h>
#include <mem/stdmem.h>
#include <stdio.h>
#include <string.h>
#include <timers/pit/pit.h>
#include <mem/paging.h>
#include <maths.h>
#include "apps/tick-monitor.h"
#include <logging/logging.h>
#include <loggingLevel.h>

void loop(volatile uint32_t *start, uint32_t count) {
    kprintf("Running Count\r\n");
    *(uint32_t *)start = 0;
    outNumTicks();
    pauseTicks(1);  // so that we are starting on a tick
    for (uint32_t i = 0; i < count; i++) {
        *(start + mod(i, 4) * sizeof(uint32_t)) = i;
        loggingWait(); // pause for the PIT if needed
        
        int j = *(start + mod(i, 4) * sizeof(uint32_t));
        
        loggingWait();
    }
    outNumTicks();
}


void mallocTest() {
    kprintf("\r\nRunning mallocTest\r\n");
    char *addr = kmalloc(1);
    // volatile char *otherAddr = kmalloc(1);

    kprintf("   got addr 0x%x\r\n", addr);
    // kprintf("STRCMP: %d\r\n", strcmp("abc", "abc"));
    // listPresentPages();
    do {
        kprintf("\r\n->");
        memset(addr, 0, 1024); // this was causing issues due to the repeated writes -- it really shouldnt be but it was just giving a lot of was 0 now 0 in the logs and crashing
        getStr(addr, 1024, 1);
        #if _SERIAL_LOGGING_LEVEL >= _LOGGING_INFO
            sprintf("[RCVD]\r\n");
            sprintf("You Wrote: -- ");
            sprintf("%s\r\n", addr);
            // sprintf("\r\n***\r\n\r\n");
            // sprintf("STRLEN: %d\r\n", strlen(addr));
        #endif

        if (strcmp(addr, "count") == 0) {
            loop((uint32_t *)addr, 1000);
        }
        #if _SERIAL_LOGGING_LEVEL >= _LOGGING_INFO
            sprintf("\r\n****\r\n\r\n");
        #endif
        // sprintf("Running into the while\r\n");
    } while (strcmp(addr, "exit"));
    kprintf("\r\n");
    free((uint32_t *)addr);
}