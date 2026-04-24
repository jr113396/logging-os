#include "logging.h"
#include <stdlib.h>
#include <stdio.h>
#include <mem/kmalloc.h>
#include <drivers/serial/serial.h>
#include <timers/pit/pit.h>
#include <mem/paging.h>
#include <loggingLevel.h>

static logging_data *head = NULL;
static uint8_t loggingEnabled = 1;

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
        #if _PRINT_LOGGING_LEVEL >= _LOGGING_INFO
            kprintf("@%d 0x%x: %d\r\n", data->numTicks, data->addr, data->after);
        #endif
        #if _SERIAL_LOGGING_LEVEL >= _LOGGING_INFO
        sprintf("@%d 0x%x: %d\r\n", data->numTicks, data->addr, data->after);
        #endif
        #if JSON_LOGS == 1
            char *type = (data->type == WRITE) ? "WRITE"  : "READ";
            char *device = (data->device) ? "MEMORY" : "OTHER";
            char *userType = (data->userType == USER) ? "USER" : "SUPERVISOR";


            
            lprintf(""
                "{"
                    "\"addr\":%ud, "
                    "\"before\": %ud, "
                    "\"after\": %ud, "
                    "\"numTicks\": %ud, "
                    "\"instAddr\": %ud, "
                    "\"type\": \"%s\", "
                    "\"device\": %s, "
                    "\"userType\": %s "
                "},\r\n",
                data->addr,
                data->before,
                data->after,
                data->numTicks,
                data->instAddr,
                type,
                device,
                userType
            );
            /*
            
            {
                "addr":addr,
                "before":before,
                "after":after,
                "numTicks":numTicks,
                "type":type,
                "device":device,
                "userLevel":userType
            },
            
            
            
            */
        # else
            lprintf("@%d 0x%x: %d\r\n", data->numTicks, data->addr, data->after);
        #endif
    } else {
        #if _PRINT_LOGGING_LEVEL >= _LOGGING_INFO
            kprintf("@%d 0x%x: %d -> %d\r\n", data->numTicks, data->addr, data->before, data->after);
        #endif
        #if _SERIAL_LOGGING_LEVEL >= _LOGGING_INFO
            sprintf("@%d 0x%x: %d -> %d\r\n", data->numTicks, data->addr, data->before, data->after);
        #endif
        
        #if JSON_LOGS == 1
            char *type = (data->type == WRITE) ? "WRITE"  : "READ";
            char *device = (data->device == MEMORY) ? "MEMORY" : "OTHER";
            char *userType = (data->userType == USER) ? "USER" : "SUPERVISOR";


            lprintf(""
                "{"
                    "\"addr\":%ud, "
                    "\"before\": %ud, "
                    "\"after\": %ud, "
                    "\"numTicks\": %ud, "
                    "\"instAddr\": %ud, "
                    "\"type\": \"%s\", "
                    "\"device\": \"%s\", "
                    "\"userType\": \"%s\" "
                "},\r\n",
                data->addr,
                data->before,
                data->after,
                data->numTicks,
                data->instAddr,
                type,
                device,
                userType
            );
        # else
            lprintf("@%d 0x%x: %d -> %d\r\n", data->numTicks, data->addr, data->before, data->after);
        #endif
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

void loggingWait() {
    // if breakpoints are enabled then skip this pause, if not pause for the pit to catchup
    #if BREAKPOINTS_ENABLED == 1 
        // do nothing 
    #else
        // otherwise using the PIT to pause 1 tick to let the PIT catch up
        #ifndef TEST_CASE_3
            pauseTicks(1);
        #endif
    #endif
}

uint8_t isLoggingEnabled() {
    return loggingEnabled;
}

void setLoggingEnabled(uint8_t isEnabled) {
    loggingEnabled = isEnabled;
}

void enableLogging() {
    sprintf("Logging Enabled\r\n");
    setLoggingEnabled(1);
    disablePages();
}
void disableLogging() {
    sprintf("Logging Disabled\r\n");
    setLoggingEnabled(0);
}