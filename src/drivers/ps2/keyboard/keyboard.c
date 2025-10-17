#include "keyboard.h"
#include "keymap.h"

#include <interupts/idt.h>
#include <io/kprintf.h>
#include <io/io.h>
#include <io/vga.h>
#include <stdio.h>



bool    capslockOn  = 0,
        shiftOn     = 0,
        charRead    = 0;

char lastChar = ' ';

char currentPressedKeys[128] = {0}; // all the keys currently pressed down, 0 for no, 1 for yes 
                                    // -> uses scan code not ascii 
                                    // -> use charToCode to find scan code before checking this 

void initKeyboard() {
    install_irq_handler (1, &keyboardHandler); 
}

void keyboardHandler(struct InterruptRegisters *regs) {
    uint8_t code = inb(0x60) & 0x7F; // what key is pressed
    uint8_t pressed = (inb(0x60) & 0x80) >> 7; // if the key is pressed or released
    uint8_t chr;
    if (code < 128) {
        if (pressed == 0) 
            currentPressedKeys[code] = 1;
        else
            currentPressedKeys[code] = 0;
    }
    switch (code) {
        case 42: // Lshift
        case 54: // Rshift
            if (pressed == 0) {
                shiftOn = true;
            }
            else  {
                shiftOn = false;
            }
            break;


        case 58: // caps lock
            if (pressed)
                capslockOn = !capslockOn;
            break;
        
        default:
            if (pressed) {
                if (!shiftOn && !capslockOn) {
                    // lower case as neither on
                    chr = lowercaseMap[code];
                    break;
                }
                if (!shiftOn && capslockOn) {
                    // yes caps lock, no shift 
                    chr = capsMap[code];
                    break;
                }
                if (shiftOn && !capslockOn) {
                    // yes shift, no caps
                    chr = shiftMap[code];
                    break;
                }
                if (shiftOn && capslockOn)  {
                    // yes shift and caps
                    chr = shiftCapsMap[code];
                    break;
                }
            }
    }
    if (pressed) {
        lastChar = chr;
        charRead = false;
    }
    // kprintf("Key: %c, %d, pressed: %b\n", chr, code, pressed);
}

char getChar() {
    while (1) {
         // keep waiting until you get a char
        if (!charRead) {
            charRead = true;
            return lastChar;
        }
    }
}

uint8_t charToCode(uint8_t c) {
    // return the scan code for a given char
    int pos = 0;
    // check lowercase first
    uint8_t *map = lowercaseMap;
    for (pos = 0; pos < 128; pos++) {
        if (map[pos] == c) {
            return pos;
        }
    }
    // then shift
    map = shiftMap;
    for (pos = 0; pos < 128; pos++) {
        if (map[pos] == c) {
            return pos;
        }
    }
    // if not found, return UNMAP as not in given keymap
    return UNMAP;
}

// Takes **ASCII** value not **SCANCODE**
int isKeyPressed(uint8_t c) {
    // returns if a current
    return currentPressedKeys[charToCode(c)];
}
