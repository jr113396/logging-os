#ifndef __drivers_serial_serial_h
#define __drivers_serial_serial_h

#include <stdint.h>

static uint16_t PORT_MAPPING[] = {0x3F8, 0x2F8, 0x3E8, 0x2E8, 0x5F8, 0x4F8, 0x5E8, 0x4E8};

int initSerial(int serialNum); 
int serialWriteChar(int serialNum, char c);
void serialPutChar(char c); 
#endif
