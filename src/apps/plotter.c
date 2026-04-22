#include "plotter.h"

#include "io/vga.h"
#include <stdio.h>
#include "timers/pit/pit.h"
#include <stdint.h>
#include <maths.h>
#include <random.h>
#include <loggingLevel.h>

int height = 23,
    width = 80;

void clearColumn(int columnID) {
    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
        sprintf("Clearing Column %d\r\n", columnID);
    #endif
    for (int i = 0; i < height; i++)
        putCharAtPos(' ', columnID, i,  WHITE, terminal_bg_colour);
}

int getNextVal() {
    uint64_t ticks = getTickCount() << 0xC00000;
    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
        sprintf("Ticks %d\r\n", ticks);
    #endif
    return mod(ticks, 50); 
}

int getPercentage(int val, int minVal, int maxVal) {
    return min((val * 100) /(maxVal-minVal), 100);
}

void plotVal(int percent, int column) {
    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
        sprintf("Putting at %d %d\r\n",column, (percent * height / 100));
    #endif
    putCharAtPos('@', column, (percent * height / 100), GREEN, BLUE);
}

void initPlotter() {
    int column = 0;
    setSeed(getTickCount() ^ 0x2345);
    while (1) {
        column++;
        if (column >= width)
            column = 0;

        clearColumn(column);
        int val = getNextVal();
        int perc = getPercentage(val, 1, 50);
        #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
            sprintf("Got Val: %d\n", val);
            sprintf("Got Percentage: %d for col %d\r\n", perc, column);
        #endif
        plotVal(perc, column);
        pauseTicks(18);
    }
}
