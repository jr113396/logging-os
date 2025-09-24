#include "echo.h"
#include "io/kprintf.h"
#include "io/vga.h"
#include "drivers/ps2/keyboard/keyboard.h"

void echo() {
    for (;;) {
        uint8_t c = getChar();
        if (c > 128) {
            // special char
            switch (c) {
                case 0xFF - 23: // up 
                {
                    int xLoc = terminalGetX();
                    int yLoc = terminalGetY();
                    status("X - %d, Y-%d\n", xLoc, yLoc);
                    terminalGotoPos(xLoc, --yLoc);
                }   
                   break;

                case 0xFF - 24: // down
                {
                    int xLoc = terminalGetX();
                    int yLoc = terminalGetY();
                    status("X - %d, Y-%d\n", xLoc, yLoc);

                    terminalGotoPos(xLoc, ++yLoc);
                }
                    break;
                    
                case 0xFF - 25: // left
                {
                    int xLoc = terminalGetX();
                    int yLoc = terminalGetY();
                                        status("X - %d, Y-%d\n", xLoc, yLoc);

                    terminalGotoPos(--xLoc, yLoc);
                }
                    break;
                    
                case 0xFF - 26: // right
                {
                    int xLoc = terminalGetX();
                    int yLoc = terminalGetY();                    status("X - %d, Y-%d\n", xLoc, yLoc);

                    terminalGotoPos(++xLoc, yLoc);
                }
                    break;
            }
        }
        else 
            kprintf("%c", c);
    }
}