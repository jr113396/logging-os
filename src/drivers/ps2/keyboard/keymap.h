#ifndef keymaph
#define keymaph

#include "stdint.h"



#define UNMAP    0xFF - 0
#define ESC      0xFF - 1
#define LCTRL    0xFF - 2
#define LSHIFT   0xFF - 3
#define RSHIFT   0xFF - 4
#define ALT      0xFF - 5
#define SPACE    ' '//0xFF - 6
#define CAPS     0xFF - 7
#define F1       0xFF - 8
#define F2       0xFF - 9
#define F3       0xFF - 10
#define F4       0xFF - 11
#define F5       0xFF - 12
#define F6       0xFF - 13
#define F7       0xFF - 14
#define F8       0xFF - 15
#define F9       0xFF - 16
#define F10      0xFF - 17
#define F11      0xFF - 18
#define F12      0xFF - 19
#define NUMLCK   0xFF - 20
#define SCRLLCK  0xFF - 21
#define HOME     0xFF - 22
#define UP       0xFF - 23
#define DOWN     0xFF - 24
#define LEFT     0xFF - 25
#define RIGHT    0xFF - 26
#define PAGEUP   0xFF - 27
#define PAGEDN   0xFF - 28
#define END      0xFF - 29
#define INS      0xFF - 30
#define DEL      0xFF - 31

extern const uint8_t lowercaseMap[128];
extern const uint8_t shiftMap[128];
extern const uint8_t capsMap[128];
extern const uint8_t shiftCapsMap[128];
#endif