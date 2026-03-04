#include "logging.h"
#include <stdlib.h>
#include <stdio.h>
#include <mem/kmalloc.h>
#include <drivers/serial/serial.h>

static logging_data *head = NULL;

void insertLog(logging_data *data) {
    if (!head)
        head = data;
    else {
        logging_data *pos = head;
        while (pos != NULL) {
            if (!pos->next)
                break;
            pos = pos->next;
        }
        pos->next = data;
    }
    
}
void readLog(logging_data * data) {
    if (data->type == READ) {
        kprintf("@%d 0x%x: %d\r\n", data->numTicks, data->addr, data->after);
        sprintf("@%d 0x%x: %d\r\n", data->numTicks, data->addr, data->after);


        
        lprintf("@%d 0x%x: %d\r\n", data->numTicks, data->addr, data->after);

    } else {
        sprintf("@%d 0x%x: %d -> %d\r\n", data->numTicks, data->addr, data->before, data->after);
        kprintf("@%d 0x%x: %d -> %d\r\n", data->numTicks, data->addr, data->before, data->after);
        lprintf("@%d 0x%x: %d -> %d\r\n", data->numTicks, data->addr, data->before, data->after);

    }
}
void readLogs() {
    kprintf("\r\n-- Logs -- \r\n");
    logging_data *data = head;
    uint32_t numDisplayed = 0;
    while (data != NULL) {
        if (numDisplayed > 15) {
            if (getChar() == 'q') {
                break;
            }
            numDisplayed--;
        }
        readLog(data);
        numDisplayed ++;
        if (data->next == NULL)
            break;
        data = data->next;
    }
    kprintf("-- Logs Finished -- \r\n");
}

void clearLogs() {
    logging_data *data = head,
                  *next;
    while (data != NULL) {
        next = data->next;
        free((uint32_t *)data);
        if (data->next == NULL) {
            break;
        }
        data = next;
    }
    head = NULL;
}

void countLogs() {
    uint32_t num = 0;
    logging_data *data = head;
    while (data != NULL) {
        num++;
        data = data->next;
    }
    kprintf("\r\n[COUNTLOGS]: %d logs\r\n", num);
}

void outLoggingSerialChar(char c) {
    // if we have multiple serial ports 
    #ifdef multiserial
        // output to the second one
        serialWriteChar(2, c);
    #else
        // otherwise output to the default one 
        serialPutChar(c);
    #endif
}

void lprintf(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    kfprintf(outLoggingSerialChar, fmt, ap);
    va_end(ap);
}
