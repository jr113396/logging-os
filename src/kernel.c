#include "globals.h"
#include "multiboot.h"

void abort( ) {
   kprintf ( "\n**** ABORT! ****\n");
   kprintf ( "Processor halted\n");
   asm ( "hlt;\n\t" );
}

void kmain( void* mbd, unsigned int magic ) {
   vgainit  ( );

   clearScreen();

   for (int i = 0 ; i < 100;i++) {
      kprintf("%d,",i);
   }


   kprintf("\nprinting: %d", 12);
   // getbootinfo (mbd, magic);
   // abort( );
}
