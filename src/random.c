#include "random.h"
#include <maths.h>

uint32_t randomSeed;

uint32_t randint() {
    uint32_t a = 16807;
    uint32_t m = 2147483647;
    randomSeed = mod((a * randomSeed), m);
    return randomSeed / m;
}

void setSeed(uint32_t val) {
    randomSeed = val;
}