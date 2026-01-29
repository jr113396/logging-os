#include "rtc.h"
#include <io/io.h>
#include <stdio.h>
#include <io/vga.h>

#include "interupts/idt.h"
#include "interupts/interuptRegs.h"

// registers
// 0x00 - Seconds
// 0x02 - Minutes
// 0x04 - Hours
// 0x06 - Weekday
// 0x07 - Day of the Month
// 0x08 - Month
// 0x09 - Year
// 0x0A - Reg A
// 0x0B - Reg B
enum RTC_REG {
    SECONDS = 0x00,
    MINUTES = 0x02,
    HOURS   = 0x04,
    WEEKDAY = 0x06,
    DAYMONTH= 0x07,
    MONTH   = 0x08,
    YEAR    = 0x09,
    REG_A   = 0x0A,
    REG_B   = 0x0B
};

uint8_t readRegisterNumber(uint8_t regNum) {
    outb(0x70, (1 << 7) | regNum);  // write a register to read 
    uint8_t val = inb(0x71);        // read the value from that register
    return val;                     // and return that value
}

datetime getCurrentTime() {
    // get current time 
    // seconds
    uint8_t seconds = readRegisterNumber(SECONDS);
    uint8_t minutes = readRegisterNumber(MINUTES);
    uint8_t hours   = readRegisterNumber(HOURS);
    uint8_t weekday = readRegisterNumber(WEEKDAY);
    uint8_t day     = readRegisterNumber(DAYMONTH);
    uint8_t month   = readRegisterNumber(MONTH);
    uint8_t year    = readRegisterNumber(YEAR);
    uint8_t regB    = readRegisterNumber(REG_B); 
    /*
    if (hours & (1 << 7) != 0) {
        kprintf("PM not AM\r\n");
        // if the highest bit is set 
        // then it is pm not am 
        // in 12 hr mode
        hours &= ~(1<<7); // so clear that bit
                          // and get the new hour
    }
    */

    // conversions from https://wiki.osdev.org/CMOS
    if (!(regB & 0x04)) {
        // convert values from BCD
        seconds = (seconds & 0x0F) + ((seconds/16) * 10);
        minutes = (minutes & 0x0F) + ((minutes/16) * 10);
        hours = ( ((hours & 0x0F) + (((hours & 0x70 / 16) * 10))) | (hours & 0x80));

        day = (day & 0x0F) + ((day / 16) * 10);
        month = (month & 0x0F) + ((month / 16) * 10);
        year = (year & 0x0F) + ((year / 16) * 10);
    }

    datetime time;

    time.seconds = seconds;
    time.minutes = minutes;
    time.hours   = hours;
    time.weekday = weekday;
    time.day     = day;
    time.month   = month;
    time.year    = year;
    
    return time;
}

void printTime() {
    datetime time = getCurrentTime();
   
    char *day[] = {"", "Sun", "Mon", "Tue", "Wed", "Thur", "Fri", "Sat"};
    lStatus("%2d:%2d:%2d - %s - %2d-%2d-20%2d", time.hours, time.minutes, time.seconds, day[time.weekday], time.day, time.month, time.year);
}

void rtcHandler(struct InterruptRegisters *regs) {
    outb(0x70, 0x0c); // read to clear the interupts
    inb(0x71); // dont save the contents
    printTime();
}


void initRTC() { 


    cli(); // disable interupts

    outb(0x70, 0x8B);
    char prev = inb(0x71);

    outb(0x70, 0x8B);
    outb(0x71, prev | 0x40);
 
 

    // with help from https://wiki.osdev.org/RTC
    outb(0x70, 0x8A); // select reg b 

    prev = inb(0x71); 
    

    uint8_t rate = 15; // 3 - 15 or 0 to disable the interupts
                      // 1 - 0.81 ms 
                      // 2 - 3.91 ms 
                      // 3 - 122 uS - 8 kHz -- fastest speed
                      // 15 - 0.5 s

    uint8_t frequency = 32768 >> (rate - 1); // set the frequency

    // rate &= (0x0F); // must be > 2 and < 15

    
    outb(0x70, 0x8A); 
    outb(0x70, 0x8A); 

    outb(0x71, (prev & 0xF0) | rate ); // set the lowest 4 bits to the rate
    install_irq_handler(8, &rtcHandler); // install the irq handler for the rtc

    sti(); // enable interupts
     
}
