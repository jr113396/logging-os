#include "globals.h"
#include "multiboot.h"
#include "gdt/gdt.h"
#include "io/vga.h"
#include "interupts/idt.h"
#include "pit/pit.h"
#include "drivers/ps2/keyboard/keyboard.h"
#include "drivers/serial/serial.h"
#include "apps/echo.h"
#include "apps/terminal.h"
#include "apps/plotter.h"
#include "mem/paging.h"
#include "mem/physicalMemManager.h"
#include "protected.h"
#include <string.h>
#include <stdio.h>

void abort( ) {
   kprintf ( "\n**** ABORT! ****\n");
   kprintf ( "Processor halted\n");
   asm ( "hlt;\n\t" );
}

void kmain( void* mbd, unsigned int magic ) {
    int returnedCodes = 0;
    vgainit( );
    clearScreen();
    initGdt();
    setIDT();
    initPit();
    initKeyboard();
    returnedCodes += initSerial(1);

    sprintf("Starting Protected Mode \r\n");
    enterProtectedMode();
    sprintf("Protected Mode Enabled\r\n");

    initPaging();
    sprintf("paging initalised\r\n");

    contigousMemStruct freeMem = getLargestMemory(mbd, magic);
    sprintf("Free Mem Found %d - %d -> %d Kb\r\n", freeMem.start, freeMem.end, ((freeMem.length/(1024 * 4))/4));
    sprintf("Setting Mem Start to %ud\r\n", freeMem.start);
    setMemStart(freeMem.start  + 200000);

    uint32_t *addr, base = 0xC000000;
    uint32_t a;

    sprintf("TESTING VALUES\r\n");
    addr = 0x40000;
    sprintf("Reading from %ux\r\n", addr);
    a = *addr;
    sprintf("Writing to val %ux\r\n", addr);
    *addr = 10; 
    
    for (int i = 0; i <10; i++) {
        addr = (base + i);
        sprintf("Reading from %ux - i %d\r\n", addr, i);
        a = *addr;
        sprintf("Writing to val %ux\r\n", addr);
        *addr = 10;
    }

    base = 0x400000;
    for (int i = 0; i <10; i++) {
        addr = (base + i);
        sprintf("Reading from %ux - i %d\r\n", addr, i);
        a = *addr;
        sprintf("Writing to val %ux\r\n", addr);
        *addr = 10;
    }
    base = 0x800000;
    for (int i = 0; i <10; i++) {
        addr = (base + i);
        sprintf("Reading from %ux - i %d\r\n", addr, i);
        a = *addr;
        sprintf("Writing to val %ux\r\n", addr);
        *addr = 10;
    }
    base = 0xA00000;
    for (int i = 0; i <10; i++) {
        addr = (base + i);
        sprintf("Reading from %ux - i %d\r\n", addr, i);
        a = *addr;
        sprintf("Writing to val %ux\r\n", addr);
        *addr = 10;
    }

    uint32_t *pageAddr = getPage();
    viewMem(pageAddr, 4 * 1024);
    char *msg = "This is a test thing to try and put in a page and see if it works\0";
    memcpy(pageAddr, msg, strlen(msg));
    kprintf("From Page: %s\r\n", pageAddr);

    returnedCodes++;
    // uint32_t *addr = allocNewPage();
    // sprintf("Allocated addr %ud\r\n", addr);

    // *addr = 10;

    // sprintf("Set value to 10: %d at %d\r\n", *addr, addr);

    // clearScreen();

    // for (int i = 0 ; i < 26;i++) {
    //    kprintf("%d\n",i);
    // }
    // for (int i = 0; i < 8; i++) {
    //    kprintf("%c", 'A' + i);
    //    for (int j = 1; j < 10; j++) {
    //       kprintf("%d", j);
    //    }
    // }
    // kprintf("THis is a long message to test something\n");
    // kprintf("short\n");   
    
    // uint32_t *addr = 0xC000000;
    // *addr = 10;

    // for (;;);
    status("3rd Year MyOs Project - Jamie Ross");
    // getbootinfo (mbd, magic);
    // sprintf("--------\r\n");

    // abort( );
    // echo();
    if (returnedCodes == 0) {
        sprintf("Starting terminal\r\n");
        initTerminal();
    } else {
        kprintf("Got status codes: %d from init functions\n", returnedCodes);
    }
    abort();
}

void kprotectedMain() {
    // i thought this would run when i entered protected mode but it doesnt seem to? but ill just leave this here in case
    abort();
}