#ifndef __GLOBALS__
#define __GLOBALS__

#define BREAK(); asm volatile ( "xchg %bx, %bx;" ); /* Bochs magic break code */
#define NULL 0

#define STRING_BUFFER_SIZE  64
#define ctrlreg             0x03D4 // Address on I/O bus
#define datareg             0x03D5 // Address on I/O bus


//
//	=================== PUBLIC FUNCTIONS ===================
//

// kernel.c
void kmain    ( void *, unsigned int );
void abort    ( void );

// multiboot.c
void getbootinfo (void *, unsigned int );

#endif //__GLOBALS__
