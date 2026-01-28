#include "terminal.h"
#include "echo.h"

#include "gameOfLife.h"
#include "app.h"
#include "tick-monitor.h"

#include <io/vga.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <mem/stdmem.h>
#include <timers/pit/pit.h>

struct appStruct apps[5];
uint8_t noCommands = 5;


void rstTerminal() {
    clearScreen();
    terminalGotoPos(0,0);
    kprintf("TERMINAL\n");
}

void helpTerminal() {
    kprintf("\nAvailable Commands are:\n");
    for (int app = 0; app < noCommands; app++) {
        kprintf("%s\n",apps[app].help);
    }
}

/*
struct appStruct {
    char *names[];
    uint8_t noNames;
    char *help;
    void *funcs;
    uint8_t rstAfter;
};
*/

void setApp(struct appStruct apps[], uint8_t pos, char **appNames, uint8_t noNames, char *help, void *func, uint8_t rstAppRet) {
    apps[pos].names = appNames;
    apps[pos].noNames = noNames;
    apps[pos].help = help;
    apps[pos].func = func;
    apps[pos].rstAfter = rstAppRet;
}

void initTerminal() {
    char buffer[81];
    char *app1Names[] = {"echo"};
    char *app2Names[] =  {"clear", "cls", "rst"};
    char *app3Names[] = {"help"};
    char *app4Names[] = {"game", "game of life"};
    char *app5Names[] = {"ticks"};
    setApp(apps, 0, app1Names, 1, "Echo \t-> Enter the echo mode", &echoInit, 1);
    setApp(apps, 1, app2Names, 3,  "Clear \t-> Clears the screen and resets the cursor", &rstTerminal, 0);
    setApp(apps, 2, app3Names, 1, "Help \t-> print help for all apps", &helpTerminal, 0);
    setApp(apps, 3, app4Names, 2, "Game \t-> an implmentation of the game of life", &initGameOfLife, 1);
    setApp(apps, 4, app5Names, 1, "Ticks \t-> Displays the current number of ticks", &outNumTicks, 0);
    // char *commands[] = {"echo", "clear", "help", "game"};
    // void *cmdFuncs[] = {
    //     &echoInit,
    //     &rstTerminal,
    //     &helpTerminal,
    //     &initGameOfLife
    // };
    uint8_t cmdFound = 0;
    clearScreen();
    kprintf("TERMINAL\n");
    getChar();
    for (;;) {
        cmdFound = 0;
        memset(buffer, 0, 80);
        
        kprintf(">");
        
        uint16_t noChars = getStr(buffer, 81, 1);
        for (int i = 0; i < noCommands; i++) {
            struct appStruct currentApp = apps[i];
            for (int name = 0; name < currentApp.noNames; name++) {
                if (strcmp(buffer, currentApp.names[name]) == 0) {
                    uint64_t ticks_before = getTickCount();
                    cmdFound = 1;
                    lStatus("App found %s", buffer);
                    void (*command)();
                    command = currentApp.func;
                    command();
                    if (currentApp.rstAfter) {
                        rstTerminal();
                        getChar();
                    }
                    uint64_t ticks_after = getTickCount();


                    kprintf("%s returned in %ud ticks\r\n", buffer, (ticks_after-ticks_before));
                }
            }
        }
        if (!cmdFound)
            kprintf("\nCommand Not Found '%s'\n", buffer);
    }
}

