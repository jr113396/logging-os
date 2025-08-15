#include "globals.h"

int twosComplement(int n) {
    return (n ^ 0xffffffff) + 1;
}

int strToInt(char *s) { // convert a null terminated string to an int
    int num = 0;
    char chr;
    while (*s) {
        chr = *s;
        if (chr >= '0' && chr <= '9') {
            num *= 10;
            num += chr-'0';
            // printf("a: %d\n", chr-'0');
        }            
        s++;
    }
    // printf("num: %d\n", num);
    return num;
}