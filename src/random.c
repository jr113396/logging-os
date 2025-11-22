#include "random.h"
#include <maths.h>

uint32_t seed;
uint32_t a = 16807;
uint32_t m = 2147483647;

uint32_t randint() {
    seed = mod((a * seed), m);
    return seed / m;
}

void setSeed(uint32_t val) {
    seed = val;
}