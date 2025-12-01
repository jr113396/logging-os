#include "maths.h"

uint32_t mod(uint32_t val, uint32_t div) {
    while (val >= div) {
        val-=div;
    }
    return val;
}

uint32_t max(uint32_t a,  uint32_t b) {
    if (a > b)
        return  a;
    return b;
}

uint32_t min(uint32_t a,  uint32_t b) {
    if (a < b)
        return  a;
    return b;
}