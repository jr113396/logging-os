#include "terminal.h"
#include "drivers/ps2/keyboard/keyboard.h"
#include "io/vga.h"
#include "io/kprintf.h"
#include "stdint.h"
#include "string.h"
#include "echo.h"
#include "mem/stdmem.h"

void rstTerminal() {
    clearScreen();
    terminalGotoPos(0,0);
    kprintf("TERMINAL\n");
}

void initTerminal() {
    char buffer[81];
    uint8_t noCommands = 2;
    char *commands[] = {"echo", "clear"};
    void *cmdFuncs[] = {
        &echoInit,
        &rstTerminal
    };
    uint8_t cmdFound = 0;
    clearScreen();
    kprintf("TERMINAL\n");
    
    for (;;) {
        cmdFound = 0;
        memset(buffer, 0, 80);
        
        kprintf(">");
        
        uint16_t noChars = getStr(buffer, 81);
        for (int i = 0; i < noCommands; i++) {
            if (strcmp(buffer, commands[i]) == 0) {
                cmdFound = 1;
                void (*command)();
                command = cmdFuncs[i];
                command();
            }
        }
        if (!cmdFound)
            kprintf("\nCommand Not Found '%s'\n", buffer);
    }
}

