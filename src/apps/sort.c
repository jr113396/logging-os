#include "sort.h"
#include <mem/kmalloc.h>
#include <mem/stdmem.h>
#include <string.h>
#include <stdio.h>

void sort(volatile char *chars, uint16_t len) {
    sprintf("[EnteringSort]\r\n");
    // an implementation of bubble sort to test out the logging system
    // should be simple but with lots of read/writes to memory

    uint8_t hasSwapped = 0;
    for (int j = 0; j < len - 1; j++) {
        hasSwapped = 0;
        // go through each char
        for (int i = 0; i < len - 1; i++) {
            // if this char is bigger than the next one
            if (chars[i] > chars[i+1]) {
                // swap
                chars[len+2] = chars[i+1];
                chars[i+1] = chars[i];
                chars[i] = chars[len+2];
                hasSwapped = 1;
            }
        }
        // if no chars were swapped
        if (!hasSwapped)
            // then in order so we are fine
            break;
    }
    sprintf("[Leaving Sort]\r\n");

}

void sortInit() {
    sprintf("[Entering sortInit]\r\n");
    kprintf("\r\nRunning Sort\r\n");
    char *addr = kmalloc(1);
    memset(addr, 0, 1024);
    kprintf("Enter Chars to sort\r\n> ");
    getStr(addr, 1024, 1);
    uint16_t numChars = strlen(addr);
    kprintf("Got %d chars: %s\r\n", numChars, addr);

    sort(addr, numChars);

    kprintf("Sorted Chars: %s\r\n", addr);

    sprintf("[Leaving sortInit]\r\n");
}