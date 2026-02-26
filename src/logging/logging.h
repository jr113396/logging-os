#ifndef __logging_logging_h
#define __logging_logging_h

#include <stdint.h>

typedef enum logType {
    READ,
    WRITE
} logType;

typedef struct logging_data {
    uint32_t addr; // what addr is being changed?
    uint32_t before; // what was the previous value?
    uint32_t after; // what was it changed to?
    uint32_t numTicks; // what was the clock tick value when this happened?
    logType type;
    struct logging_data *next;
} logging_data;

void insertLog(logging_data *data);
void readLog(logging_data * data);
void readLogs();
void clearLogs();
void countLogs();
#endif