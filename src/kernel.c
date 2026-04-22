#include "globals.h"
#include "multiboot.h"
#include "gdt/gdt.h"
#include "io/vga.h"
#include "interupts/idt.h"
#include "timers/rtc/rtc.h"
#include "timers/pit/pit.h"
#include "drivers/ps2/keyboard/keyboard.h"
#include "drivers/serial/serial.h"
#include "apps/echo.h"
#include "apps/terminal.h"
#include "apps/plotter.h"
#include "mem/paging.h"
#include "mem/physicalMemManager.h"
#include "mem/stdmem.h"
#include "mem/kmalloc.h"
#include "protected.h"
#include <string.h>
#include <stdio.h>
#include <loggingLevel.h>

extern char kernel_end;

void varWrite() {
    uint32_t *base = (uint32_t *) 0xE00003; // 10485763
    for (int i = 0; i <1; i++) {
        #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
            sprintf("line: %d\r\n", &varWrite);
        #endif
        uint32_t *addr = (base + i);
        #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
            sprintf("Reading from %ux - i %d\r\n", addr, i);
        #endif
        uint32_t a = *addr;
        #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
            sprintf("Writing to val %ux\r\n", addr);
        #endif

        *addr = 12;
    }
}

void abort( ) {
   kprintf ( "\n**** ABORT! ****\n");
   kprintf ( "Processor halted\n");
   asm ( "hlt;\n\t" );
}

void kmain( void* mbd, unsigned int magic ) {
    uint32_t *kernelEndAddr = (uint32_t *)&kernel_end; // start addr of useable mem

    int returnedCodes = 0;
    vgainit( );
    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_INFO
        sprintf("\r\nSTARTING\r\n");
    #endif
    clearScreen();
    initGdt();
    setIDT();
    initPit();
    initRTC();
    initKeyboard();
    returnedCodes += initSerial(1);

    #ifdef multiserial
        returnedCodes += initSerial(2);
    #endif
    
    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_INFO
        sprintf("Starting Protected Mode \r\n");
    #endif
    enterProtectedMode();
    
    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_INFO
        sprintf("Protected Mode Enabled\r\n");
    #endif

    initPaging();
    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_INFO
        sprintf("paging initalised\r\n");
    #endif
    contigousMemStruct freeMem = getLargestMemory(mbd, magic);
    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
        sprintf("Free Mem Found %d - %d -> %d Kb\r\n", freeMem.start, freeMem.end, ((freeMem.length /(1024 * 4))/4));
        sprintf("Setting Mem Start to %ud - %d bytes after grub mem start\r\n", kernelEndAddr, kernelEndAddr-freeMem.start);
    #endif
    setHigherMemStart((uint32_t *)(4 * 1024 * 1024)); // start of the 2nd page table entry
    setLowerMem(kernelEndAddr, (uint32_t *)(4 * 1024 * 1024 -1)); // end of the 1st page table entry (start of higher mem)

    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
    sprintf("Kernel End: %d\r\n\n", kernelEndAddr);
    #endif
    // varWrite();
    // installNewPageFull(0xFFFF0000,0xFFFF0000);
    // listPresentPages();
    // uint32_t *addr = kmalloc(200);
    // *addr = 300;
    // sprintf("addr: %x\r\n", addr);
    // addr = kmalloc(500);
    // *addr = 20;
    // addr = kmalloc(600);

    // *addr = 30;
    // addr = kmalloc(1000);

    // *addr = 40;
    // addr = kmalloc(1050);

    // *addr = 50;
    // addr = kmalloc(2100);
    // *addr = 60;
    // addr = kmalloc(4000);
    // uint32_t val = *addr;
    // sprintf("Val: %d\r\n", val);
    // *addr = 10;
    // val = *addr;
    // sprintf("Val: %d\r\n", val);
    // addr = kmalloc(5000);
    // listPresentPages();
    // uint32_t *addr = findNextHigherFree(1);
    // uint32_t *addr2 = findNextHigherFree(32);
    // uint32_t *addr3 = findNextHigherFree(5);
    // uint32_t *addr4 = findNextHigherFree(31);

    // sprintf("Got allocated addr: %ud\r\n", addr);
    // returnHigherPage(addr);
    // returnHigherPage(addr2);
    // returnHigherPage(addr3);
    // returnHigherPage(addr4);


    // ^^^^^ UP TO THIS POINT IS SETUP CODE ^^^^^^
    // uint32_t *addr, base = 0xC000000;
    // uint32_t a;

    // sprintf("TESTING VALUES\r\n");
    // addr = 0x40000;
    // sprintf("Reading from %ux\r\n", addr);
    // a = *addr;
    // sprintf("Writing to val %ux\r\n", addr);
    // *addr = 10; 
    
    // // for (int i = 0; i <10; i++) {
    //     addr = (base + i);
    //     sprintf("Reading from %ux - i %d\r\n", addr, i);
    //     a = *addr;
    //     sprintf("Writing to val %ux\r\n", addr);
    //     *addr = 10;
    // }

    // base = 0x400000;
    // for (int i = 0; i <10; i++) {
    //     addr = (base + i);
    //     sprintf("Reading from %ux - i %d\r\n", addr, i);
    //     a = *addr;
    //     sprintf("Writing to val %ux\r\n", addr);
    //     *addr = 10;
    // }
    // base = 0x800000;
    // for (int i = 0; i <10; i++) {
    //     addr = (base + i);
    //     sprintf("Reading from %ux - i %d\r\n", addr, i);
    //     a = *addr;
    //     sprintf("Writing to val %ux\r\n", addr);
    //     *addr = 10;
    // }
   
    
    // uint32_t *pageAddr = getPage();
    // viewMem(pageAddr, 4 * 1024);
    // char *msg = "This is a test thing to try and put in a page and see if it works\0";
    // memcpy(pageAddr, msg, strlen(msg));
    // kprintf("From Page: %s\r\n", pageAddr);
    // viewMem(pageAddr, 4 * 1024);

    // returnedCodes++;
    // sprintf("kernel start: %d\r\n", kernel_start);
    // sprintf("kernel end: %d\r\n", kernel_end);

    // for (;;);
    status("3rd Year MyOs Project - Jamie Ross");
    // getbootinfo (mbd, magic);
    // sprintf("--------\r\n"); 
    // returnedCodes++;
    // abort( );
    // echo();
    if (returnedCodes == 0) {
        #if _SERIAL_LOGGING_LEVEL >= _LOGGING_INFO
        sprintf("\r\n\r\nStarting terminal\r\n\r\n\n\r\n");
        #endif
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
