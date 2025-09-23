#include "echo.h"
#include "io/kprintf.h"
#include "drivers/ps2/keyboard/keyboard.h"

void echo() {
    for (;;) {
        char c = getChar();
        kprintf("%c", c);
    }
}