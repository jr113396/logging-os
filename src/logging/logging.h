#ifndef __logging_logging_h
#define __logging_logging_h

#include <stdint.h>

typedef enum logType {
    READ,
    WRITE
} logType;

typedef enum deviceType { // what device caused the log - memory / disk etc
    MEMORY
} deviceType;

typedef enum userLevel {
    USER,
    SUPERVISOR
} userLevel;

typedef struct logging_data {
    uint32_t addr; // what addr is being changed?
    uint32_t before; // what was the previous value?
    uint32_t after; // what was it changed to?
    uint32_t numTicks; // what was the clock tick value when this happened?
    logType type; // read or write
    deviceType device; // memory / disk 
    userLevel userType; // user or supervisor
    struct logging_data *next;
} logging_data;

void insertLog(logging_data *data);
void readLog(logging_data * data);
void readLogs();
void clearLogs();
void countLogs();
#endif