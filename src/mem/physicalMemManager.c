#include "physicalMemManager.h"

#include "paging.h"
#include <stdio.h>
#include "stdmem.h"
#include <maths.h>

uint32_t bitmap[LENGTH_OF_BITMAP] = {0};
uint32_t *lastAccessed = bitmap;
uint32_t *memStart = 0, // this needs to be set
         *lowerMemStart = 0; // this actually does start at 0 - confusing i know
char *a = "SOMETHING";

uint32_t lowerBitmap[LENGTH_OF_LOWER_MAP] = {0};

void viewMem(uint8_t *addr, uint32_t length) {
    sprintf("\r\n");
    sprintf("******************************\r\n");
    sprintf("*                            *\r\n");
    sprintf("*    Mem Map View Start      *\r\n");
    sprintf("*                            *\r\n");
    sprintf("******************************\r\n");

    sprintf("Mem Map of area %d - %x for %d bytes\r\n", addr, addr, length);
    for (uint32_t i = 0; i < length; i++) {
        if (i % 50 == 0) 
            sprintf("\r\n%x - | ", (addr + i));
        switch (addr[i]) {
            case '\t':
                sprintf("\\t");
                break;
            case '\r':
                sprintf("\\r");
                break;
            case '\n':
                sprintf("\\n");
                break;
            default:
                sprintf("%c",addr[i]);
        }
    }
    sprintf("\r\n");

    sprintf("\r\n");
    sprintf("******************************\r\n");
    sprintf("*                            *\r\n");
    sprintf("*    Mem Map View End        *\r\n");
    sprintf("*                            *\r\n");
    sprintf("******************************\r\n");
}

void setMemStart(uint32_t * memStartAddr) {
    memStart = memStartAddr;
    // viewMem((uint32_t *)a-10000, 30000);
    sprintf("Memstart addr = %ud -> last accessed %ud, bitmap %d\r\n", &memStart, lastAccessed, a);
    memset(bitmap, 0, LENGTH_OF_BITMAP);
}


uint32_t returnPage(uint32_t *addr, uint32_t *map, uint32_t length, uint32_t *start) {
    // sets a given page in a map as free to be used by findNextFreePage
    sprintf("********Return Page Start********\r\n");
    sprintf("Addr: %ud, Start: %ud\r\n", addr, start);
    uint32_t intoMap = (addr - start);
    sprintf("Into Map: %ud\r\n", intoMap);

    uint32_t pageIndex = intoMap/PAGE_SIZE;
    sprintf("pageIndex: %d\r\n\n", pageIndex);

    uint32_t mapIndex = pageIndex / 32;
    sprintf("mapIndex: %d\r\n", mapIndex);
    uint8_t bitPos = mod(pageIndex, 32);
    sprintf("bitPos: %d\r\n", bitPos);

    uint8_t avlBits = ((uint32_t) addr >> 8) & (0b111);
    sprintf("Got avlBits %ub \r\n", avlBits);
    
    uint32_t mapVal = bitmap[mapIndex] & (1 << bitPos);
    sprintf("Looked in index: %d at pos %d and found val %d\r\n", mapIndex, bitPos, mapVal);
    if (mapVal == 0) {
        sprintf("[return page] Map val found == 0? maybe double freed\r\n");
        return 0;
    }
    else {
        // set the bit back to 0 as now free
        bitmap[mapIndex] ^= (1 << bitPos);
    }


    // page now freed
    
    // int pageNum = (i * 4 * 8) + (bitPos); 
    
    return 1;
}

uint32_t returnHigherPage(uint32_t *addr) {
    return returnPage(addr, bitmap, LENGTH_OF_BITMAP, memStart);
}

uint32_t *findNextFreePage(uint8_t numPages, uint32_t *map, uint32_t length, uint32_t *start) {
    // returns the addr of the next free page given a bitmap, length of the bitmap and where the start of the map maps to in memory
    
    if (numPages > 32) {
        sprintf("Incorrect number of pages requested %d, must be 1-32\r\n", numPages);
        return 0;
    }
    
    sprintf("\r\nAlocating %d pages\r\n", numPages);
    uint32_t mask = 0;
    for (int i = 0; i < numPages; i++) {
        mask |= (1 << i);
    }

    
    for (uint32_t i = 0; i < length; i++){
        sprintf("%ud - %ub\r\n", map + i, map[i]);



        if ((map[i] & 0xFFFFFFFF) == 0xFFFFFFFF) {
            // all pages aready allocated so skip it

            // sprintf("All Fs at addr %ud -- %b\r\n", map+i, map[i]);
            continue;
        } else {
            // we have some space here to put a page in
            // sprintf("Found a none full byte - %b\r\n", map[i]);


            for (int bitPos = 0; bitPos <= 32 - numPages; bitPos++) {
                // sprintf("Runnign %b got %d\r\n", 1<<bitPos, map[i]  & (1 << bitPos) );
                // TODO - Fix overflowing on shift
                if ((map[i]  & (mask << bitPos)) == 0) {
                    sprintf("Bit Pos: %d \r\n, map[i]: %ub\r\n", bitPos, map[i]);
                    // sprintf("Found free bit %d\r\n", bitPos);
                    // found a free space
                    map[i] |= (mask << bitPos); // set that bit to 1 to show that its in use
                    sprintf("Mask shifted: %ub\r\n", mask << bitPos);
                    sprintf("Map[i] After: %ub\r\n", map[i]);
                    // get the current byte number (i * 4), * 8 as 8 pages per byte, then the bit withi that byte equals the page
                    // as each bit is representing 1 page 

                    lastAccessed = map + i;
                    int pageNum = (i * 4 * 8) + (bitPos); 
                    uint32_t *memStartLoc = start + (pageNum * PAGE_SIZE);
                    sprintf("Allocating mem at location: %d - frame %d\r\n", memStartLoc, pageNum);
                    memStartLoc = (uint32_t *)((uint32_t) memStartLoc & (~(0b111 << 9)));
                    sprintf("Set avl bits to be 110\r\n");
                    memStartLoc = (uint32_t *)(((uint32_t) memStartLoc) | (0b110 << 9));                                  


                    // sprintf("After setting avl bits %d - avl bits %ub - %ub\r\n", memStartLoc, ((uint32_t) memStartLoc), ((uint32_t) memStartLoc >> 8));

                    return memStartLoc;
                }
            }  
            sprintf("Not enough space on this not empty mapPage - %d\r\n", i);
            // return 0;
        }
    }
    return 0; // return 0 as we havent found a place
}

uint32_t *findNextLowerFree(uint8_t numPages) {
    uint32_t *addr =  findNextFreePage(numPages, lowerBitmap, LENGTH_OF_LOWER_MAP, lowerMemStart);
    sprintf("Found lower addr: %d \r\n", addr);
    
    // viewMem(addr-1000, 1000);
    // sprintf("Starts here");
    // viewMem(addr, 4 * 1024);
    // sprintf("Ends Here");
    // viewMem(addr + 4 * 1024, 1000);

    return addr;

}

uint32_t *findNextHigherFree(uint8_t numPages) {
    if (memStart == 0) {
        sprintf("Set higher addr first\r\n");
        return 0;
    }
    uint32_t *addr = findNextFreePage(numPages, bitmap, LENGTH_OF_BITMAP, memStart);

    sprintf("Found heigher addr: %d\r\n", addr);
    // viewMem(addr-1000, 1000);
    // sprintf("Starts here");
    // viewMem(addr, 4 * 1024);
    // sprintf("Ends Here");    
    // viewMem(addr + 4 * 1024, 1000);
    return addr;
}
