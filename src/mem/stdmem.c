#include "stdmem.h"

void memset(void *loc, char val, uint32_t count) {
    char *p = (char*) loc;
    for (; count > 0; count--) {
        *p++ = val;
    }
}


void *memcpy(void *dst, void *src, uint32_t count) {
    if (dst == src || count == 0)
        // already identical
        return dst;
    
    char *s = src;
    char *d = dst;
    for (int i = 0; i < count; i++)
        *d++ = *s++;
    // int num64s = count / sizeof(uint64_t);    
    // {
    //     // copy in 8 byte chunks first
    //     for (int i = 0 ; i < num64s; i++) {
    //         uint64_t *d = (uint64_t *) dst;
    //         uint64_t *s = (uint64_t *) src;
    //         *d++ = *s++; 
    //     }
    // }    
    // // at most 7 bytes remaining so copy an uint32 if possible, then uint16, then uint8
    // int remain = count % sizeof(uint64_t);

    // {
    //     int num32s = remain / sizeof(uint32_t);
    //     remain = remain % sizeof(uint32_t);
    //     uint32_t * d = dst + num64s * sizeof(uint64_t);
    //     uint32_t * s = dst + num32s * sizeof(uint32_t);
    //     if (num32s) // cpy 4 byte chunk if left
    //         *d++ = *s++;
    //     if (remain / sizeof(uint16_t)) {// cpy 2 byte chunk if left
    //         uint16_t *d16 = (uint16_t *)d;
    //         uint16_t *s16 = (uint16_t *)s;
    //         *d16++ = *s16++;
    //     }
    //     if (remain % sizeof(uint8_t)) {// and cpy last byte is left 
    //         uint8_t *d8 = (uint8_t *)d;
    //         uint8_t *s8 = (uint8_t *)s;
    //         *d8++ = *s8++;
    //     }
    // }
    // return dst;
}