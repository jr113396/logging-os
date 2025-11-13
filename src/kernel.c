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
#include "mem/paging.h"
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

    initPaging();
    sprintf("paging initalised\r\n");
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
    
    // for (;;);
    status("3rd Year MyOs Project - Jamie Ross");
    // getbootinfo (mbd, magic);
    // abort( );
    // echo();
    sprintf("Protected Mode Enabled\r\n");
    if (returnedCodes == 0) {
        sprintf("Starting terminal\r\n");
        initTerminal();
    } else {
        kprintf("Go status codes: %d from init functions\n", returnedCodes);
    }
    abort();
}

void kprotectedMain() {
}