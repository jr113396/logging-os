// #include "protected.h"
#include "io/io.h"
#include <stdio.h>
// from https://wiki.osdev.org/Non_Maskable_Interrupt
void NMI_enable() {
    outb(0x70, inb(0x70) & 0x7F);
    inb(0x71);
}

void NMI_disable() {
    outb(0x70, inb(0x70) | 0x80);
    inb(0x71);
}

void enterProtectedMode() {
    NMI_enable();

    enable_a20();

    NMI_disable();
}