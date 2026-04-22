#ifndef __logging_logging_h
#define __logging_logging_h

#include <stdint.h>
// #define TEST_CASE_3
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
    uint32_t instAddr; // what was the address of the instruction that caused this issue
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
void loggingWait();
uint8_t isLoggingEnabled();
void setLoggingEnabled(uint8_t isEnabled);
void enableLogging();
void disableLogging();
#endif