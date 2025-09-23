#include "keyboard.h"
#include "io/kprintf.h"
#include "io/io.h"
#include "interupts/idt.h"
#include "keymap.h"



bool    capslockOn  = 0,
        shiftOn     = 0;


void initKeyboard() {
    install_irq_handler (1, &keyboardHandler); 
}

void keyboardHandler(struct InterruptRegisters *regs) {
    uint8_t code = inb(0x60) & 0x7F; // what key is pressed
    uint8_t pressed = (inb(0x60) & 0x80) >> 7; // if the key is pressed or released
    char chr;
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
// if (pressed)
    kprintf("Key: %c, %d, pressed: %b\n", chr, code, pressed);
}