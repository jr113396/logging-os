#include "echo.h"
#include "io/kprintf.h"
#include "io/vga.h"
#include "drivers/ps2/keyboard/keyboard.h"
#include "drivers/ps2/keyboard/keymap.h"

void echo() {
    for (;;) {
        uint8_t c = getChar();
        status("SHIFT %d, CTRL %d", isKeyPressed(LSHIFT), isKeyPressed(LCTRL));
        if (c > 128) {
            // special char
            switch (c) {
                case UP: // up 
                {
                    int xLoc = terminalGetX();
                    int yLoc = terminalGetY();
                    terminalGotoPos(xLoc, --yLoc);
                }   
                   break;

                case DOWN: // down
                {
                    int xLoc = terminalGetX();
                    int yLoc = terminalGetY();
                    terminalGotoPos(xLoc, ++yLoc);
                }
                    break;
                    
                case LEFT: // left
                {
                    int xLoc = terminalGetX();
                    int yLoc = terminalGetY();
                    terminalGotoPos(--xLoc, yLoc);
                }
                    break;
                    
                case RIGHT: // right
                {
                    int xLoc = terminalGetX();
                    int yLoc = terminalGetY();
                    terminalGotoPos(++xLoc, yLoc);
                }
                    break;
            }
        }
        else 
            kprintf("%c", c);
    }
}