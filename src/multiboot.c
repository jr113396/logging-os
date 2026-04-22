#include "globals.h"
#include <loggingLevel.h>
#include "multiboot.h"
#include <stdio.h>

// get the contigous memory provided by the bios
contigousMemStruct getLargestMemory(void *mbd, unsigned int magic) {
   struct multiboot_info * bootinfo;
   struct memorymap *      mmap;
   struct memorymap *      mmap_end;

   contigousMemStruct mem;
   if ( magic != 0x2BADB002 ) {
      kprintf ("Multiboot ptr: %x\n", mbd);
      kprintf ("Wrong Multiboot magic number\n");
      abort ( );
   }

   bootinfo = (struct multiboot_info *)  mbd;

   if (bootinfo->flags & MBTINFO_MEMORY) {
      sprintf ("Memory: %ud - %udKB\r\n",
         bootinfo->mem_lower, bootinfo->mem_upper);
   }

 

   mem.start = 0;
   mem.end=0;
   mem.length=0;

   if (bootinfo->flags & MBTINFO_MEMORYMAP) {
      mmap     = (struct memorymap *)  bootinfo->mmap_addr;
      mmap_end = (struct memorymap *) (bootinfo->mmap_addr+bootinfo->mmap_len);
      while (mmap < mmap_end) {
         #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
            sprintf ("Memory: %08ux,%08uxh, Length: %08ux,%08uxh %s\r\n",
               mmap->base_addrh, mmap->base_addrl,
               mmap->lengthh,    mmap->lengthl,
               (mmap->type == 1) ? "Avail." : "Resvd.");
         #endif
         
         if (mmap->type == 1) {
            if (mem.length < mmap->lengthl) {
               mem.start = mmap->base_addrl;
               mem.end = mmap->base_addrl + mmap->lengthl;
               mem.length = mmap->lengthl;
               #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                  sprintf("Found new longest with start, %d, end %d, and len %d\r\n", mem.start, mem.end, mem.length);
               #endif
            }
         }

         mmap = (struct memorymap *)
                  (mmap->size + sizeof(mmap->size) + (uint32_t)mmap);
      }
   }

   return mem;
}

void
getbootinfo( void* mbd, unsigned int magic ) {
   struct multiboot_info * bootinfo;
   struct memorymap *      mmap;
   struct memorymap *      mmap_end;

   /*
    * Note the magic number we get is different to that we have in our
    * MultiBootHeader (0x1BADB002)
    */
   if ( magic != 0x2BADB002 ) {
      kprintf ("Multiboot ptr: %x\n", mbd);
      kprintf ("Wrong Multiboot magic number\n");
      abort ( );
   }

   bootinfo = (struct multiboot_info *)  mbd;

   if (bootinfo->flags & MBTINFO_MEMORY) {
      #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
      sprintf ("Memory: %ud - %udKB\r\n",
         bootinfo->mem_lower, bootinfo->mem_upper);
      #endif
   }
 
   if (bootinfo->flags & MBTINFO_CMDLINE) {
      #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
      sprintf ("Command line: %s\r\n", (char *) bootinfo->cmdline);
      #endif
   }
 
   if (bootinfo->flags & MBTINFO_MEMORYMAP) {
      mmap     = (struct memorymap *)  bootinfo->mmap_addr;
      mmap_end = (struct memorymap *) (bootinfo->mmap_addr+bootinfo->mmap_len);

      while (mmap < mmap_end) {
         
         #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
         sprintf ("Memory: %08ux,%08uxh, Length: %08ux,%08uxh %s\r\n",
            mmap->base_addrh, mmap->base_addrl,
            mmap->lengthh,    mmap->lengthl,
            (mmap->type == 1) ? "Avail." : "Resvd.");
            
         #endif
         mmap = (struct memorymap *)
                   (mmap->size + sizeof(mmap->size) + (uint32_t)mmap);
      }
   }
 
   if (bootinfo->flags & MBTINFO_BTLDRNAME) {
      #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
         sprintf ("Bootloader: %s\r\n", (char *) bootinfo->bootldrname);
      #endif
   }
 
}
