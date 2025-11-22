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


    uint32_t *addr = allocNewPage();
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
        kprintf("Go status codes: %d from init functions\n", returnedCodes);
    }
    abort();
}

void kprotectedMain() {
    // i thought this would run when i entered protected mode but it doesnt seem to? but ill just leave this here in case
    abort();
}