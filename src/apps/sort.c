#include "sort.h"
#include <mem/kmalloc.h>
#include <mem/stdmem.h>
#include <string.h>
#include <stdio.h>
#include <logging/logging.h>
#include <loggingLevel.h>

void sort(volatile char *chars, uint16_t len) {
    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
        sprintf("[EnteringSort]\r\n");
    #endif
    // an implementation of bubble sort to test out the logging system
    // should be simple but with lots of read/writes to memory

    uint8_t hasSwapped = 0;
    for (int j = 0; j < len - 1; j++) {
        hasSwapped = 0;
        loggingWait();
        // go through each char
        for (int i = 0; i < len - 1; i++) {
            // if this char is bigger than the next one
            if (chars[i] > chars[i+1]) {
                // swap
                chars[len+2] = chars[i+1];
                loggingWait();
                chars[i+1] = chars[i];
                loggingWait();
                chars[i] = chars[len+2];
                loggingWait();
                hasSwapped = 1;
                loggingWait();
            }
        }
        // if no chars were swapped
        if (!hasSwapped) {
            loggingWait(); 
            break;
        }
            // then in order so we are fine
    }
    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
    sprintf("[Leaving Sort]\r\n");
    #endif

}

void sortInit() {
    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG    
    sprintf("[Entering sortInit]\r\n");
    #endif

    kprintf("\r\nRunning Sort\r\n");
    char *addr = kmalloc(1);
    disableLogging();
    memset(addr, 0, 1024);
    enableLogging();
    kprintf("Enter Chars to sort\r\n> ");
    getStr(addr, 1024, 1);
    uint16_t numChars = strlen(addr);
    kprintf("Got %d chars: %s\r\n", numChars, addr);

    sort(addr, numChars);

    kprintf("Sorted Chars: %s\r\n", addr);
    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
        sprintf("[Leaving sortInit]\r\n");
    #endif
    free((uint32_t *)addr);
}