#include "globals.h"
#include "multiboot.h"
#include "gdt.h"

void abort( ) {
   kprintf ( "\n**** ABORT! ****\n");
   kprintf ( "Processor halted\n");
   asm ( "hlt;\n\t" );
}

void kmain( void* mbd, unsigned int magic ) {
   initGdt();
   vgainit  ( );

   clearScreen();

   for (int i = 0 ; i < 26;i++) {
      kprintf("%d\n",i);
   }
   for (int i = 0; i < 8; i++) {
      kprintf("%c", 'A' + i);
      for (int j = 1; j < 10; j++) {
         kprintf("%d", j);
      }
   }
   kprintf("THis is a long message to test something\n");
   kprintf("short\n");   

   status("3rd Year MyOs Project - Jamie Ross");
   getbootinfo (mbd, magic);
   abort( );
}
