#ifndef __timers_rtc_rtc_h
#define __timers_rtc_rtc_h

#include <stdint.h>

typedef struct datetime {
    uint8_t seconds;  // 0-59
    uint8_t minutes;  // 0-59
    uint8_t hours;    //
    uint8_t weekday;  // 1 - 7 -- 1 is sunday
    uint8_t day;      // 1-31
    uint8_t month;    // 1-12
    uint8_t year;     // 0-99

} datetime;

void initRTC();
datetime getCurrentTime();
void printTime();

#endif
