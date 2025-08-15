#ifndef __GLOBALS__
#define __GLOBALS__

#define BREAK(); asm volatile ( "xchg %bx, %bx;" ); /* Bochs magic break code */
#define NULL 0

#define STRING_BUFFER_SIZE  64
#define ctrlreg             0x03D4 // Address on I/O bus
#define datareg             0x03D5 // Address on I/O bus
//
//	======================== TYPES ========================
//
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned long      uint32_t;
typedef unsigned long long uint64_t;

typedef unsigned long      size_t;

//typedef char      int8_t;
typedef short     int16_t;
typedef long      int32_t;
typedef long long int64_t;

//
//	=================== PUBLIC FUNCTIONS ===================
//

// kernel.c
void kmain    ( void *, unsigned int );
void abort    ( void );

// io.c
void     outb    ( uint16_t, uint8_t  );

// multiboot.c
void getbootinfo (void *, unsigned int );

// printk.c
char * itoa   ( unsigned long );
int  strlen   ( char * );
void kprintf  ( char *, ... );
char *outstr(char *, int, char );
char *_itoa(unsigned long, int, unsigned int);

// vga.c
void putchar  ( char   );
void vgainit  ( void   );
// void status   ( char * );
void status(char *fmt, ...);
void clearScreen ();
void setcursor (int x, int y);
void writeError(char *error);
void putCharAtPos(char c, int xPos, int yPos, size_t fgCol, size_t bgCol);

// conversions.c
int twosComplement( int );
int strToInt(char *);

#endif //__GLOBALS__
