#include "serial.h"

#include <interupts/idt.h>
#include <stdio.h>
#include <io/io.h>
#include <loggingLevel.h>


// using infomation from https://wiki.osdev.org/Serial_Ports#Line_Control_Register


int initSerial(int serialNum) {
    if (serialNum > 8 || serialNum < 1) {
        #if _PRINT_LOGGING_LEVEL >= _LOGGING_ERROR
            kprintf("[ERROR] serial num needs to be between 1-8\n");
        #endif
        return 2;
    }

    uint16_t portAddr = PORT_MAPPING[serialNum-1];
    // default bad rate = 115200
    outb(portAddr + 1, 0x00); // disables interupts
    outb(portAddr + 3, 0x80); // enable BLAB
    outb(portAddr + 0, 0x0C); // set (low byte of) divisor for baud rate to 12 -> 9600 baud rate
    outb(portAddr + 1, 0x00); // high byte of baud rate -> 0 for this divisor
    
    outb(portAddr + 3, 0x03); // Set LCR
                            // DBPARSDA
                            // ^^^  ^^
                            // |||  |Data Bits
                            // ||\  Stop Bits
                            // |\ Partity Bits
                            // | Break Enable Bit
                            // Set DLAB

                            // 00000111 
                            // Data bits -> 11 -> 8 bit chars
                            // Stop = 1
                            // Partity = 0 

    outb(portAddr + 2, 0xC7); // enable FIFO 
                            // ITXXDTRE
                            // ^^^ ^^^
                            // | | ||Enable FIFO
                            // | | ||Clear Recv FIFO
                            // | | |Clear Transmit FIFO
                            // | | DMA Mode select
                            // | Rsvd
                            // Interupt Trigger Level
                            // Enables FIFO, clears registers
                            // 14 byte threshold on interupt

    outb(portAddr + 4, 0x0B); // Modem Control Register


    // TEST TO SEE IF WORKING 
    outb(portAddr + 4, 0x1E); // Set MCR to be in loopback mode for testing to make sure its working
                              
    outb(portAddr + 0, 0xAE); // send 0xAE byte and as we are loopback mode that should be sent back  
    if (inb(portAddr + 0) != 0xAE) {
        #if _PRINT_LOGGING_LEVEL >= _LOGGING_ERROR
            kprintf("[ERROR] error while setting up serial port %d, did not recv correct loopback data\n", portAddr);
        #endif
        return 1;
    }


    // TEST SUCCESSFULL
    outb(portAddr + 4, 0x0F); // set back to normal mode 
    return 0;
}


int writeBufferFree(uint16_t portAddr) {
    return inb(portAddr + 5) & 0x20;
}

int serialWriteChar(int serialNum, char c) {
    if (serialNum > 8 || serialNum < 1) {
        #if _PRINT_LOGGING_LEVEL >= _LOGGING_ERROR
            kprintf("[ERROR] serial num needs to be between 1-8\n");
        #endif
        return 2;
    }

    uint16_t portAddr = PORT_MAPPING[serialNum-1];

    while (writeBufferFree(portAddr) == 0);

    outb(portAddr, c);
    return 0;
}

void serialPutChar(char c) {
    serialWriteChar(1, c);
}
void sprintf(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    kfprintf(serialPutChar, fmt, ap);
    va_end(ap);
}
