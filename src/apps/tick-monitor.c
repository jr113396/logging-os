#include <stdio.h>
#include <timers/pit/pit.h>
#include "tick-monitor.h"

void outNumTicks() {
    kprintf("\nTicks: %d\r\n", getTickCount());
}
