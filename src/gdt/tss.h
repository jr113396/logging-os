#ifndef tssh
#define tssh

#include "stdint.h"

struct TSS_struct_protected {
    uint16_t LINK; // segments selector of the tss of the previous task
    uint16_t rsvdLINK;

    uint32_t ESP0; // STACK POINTER with ESP 1 and 2 - used to load the stack when a privileges level change occurs from lower to higher
    uint16_t SS0; // SEGMENT SELECTOR with SS1 and 2 - used to load the stack when a privilege level change occurs from lower to higher
    uint16_t rsvdSS0;
    
    uint32_t ESP1;
    uint16_t SS1;
    uint16_t rsvdSS1;

    uint32_t ESP2;
    uint16_t SS2;
    uint16_t rsvdSS2;

    uint32_t CR3;
    uint32_t EIP;
    uint32_t EFLAGS;

    uint32_t EAX;
    uint32_t ECX;
    uint32_t EDX;
    uint32_t EBX;
    uint32_t ESP;
    uint32_t EBP;
    uint32_t ESI;
    uint32_t EDI;

    uint16_t ES; 
    uint16_t rsvdES;
    uint16_t CS;
    uint16_t rsvdCS;
    uint16_t SS;
    uint16_t rsvdSS;
    uint16_t DS;
    uint16_t rsvdDS;
    uint16_t FS;
    uint16_t rsvdFS;
    uint16_t GS;
    uint16_t rsvdGS;
    uint16_t LDTR;
    uint16_t rsvdLDTR;

    uint16_t rsvd1;
    uint16_t IOPB; // io map base address field -> offset from base to tss to IO permission map
    uint32_t SSP;  // shadow stack pointer
    // ------------


}

#endif