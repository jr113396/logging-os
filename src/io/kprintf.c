#include "kprintf.h"
#include "vga.h"
#include <globals.h>
#include <stdarg.h>
#include <util/conversions.h>
#include <string.h>
#include <stdio.h>

// coverts a number n to a base and returns as a string
// NOTE - IF base is larger than 36, it will be set to 10
char *_itoa(unsigned long n, int isSigned, unsigned int base) {
    if (base > 36) // 0-9 + A-Z
        base = 10;
    char vals[33]; // 32 + 1 for null terminator
    static char rev_vals[33];
    int addMinusSign = 0;
    if (isSigned && (n & 0x80000000)) {
        n = twosComplement(n);
        addMinusSign = 1;
    }

    // int val = num;
    int count = 0;
    while (count < 32) {
        int whole = n/base;
        vals[count++] = (n % base);
        if (whole == 0) break;
        n = whole;
    }
    
    rev_vals[count + addMinusSign] = '\0';
    char c;

    if (addMinusSign)
        rev_vals[0] = '-';
    for (int i = 0; i < count; i++) {
        if (vals[i] < 10) 
            c = vals[i] + '0'; // convert from number to ascii
        else
            c = vals[i] - 10 + 'A';
        rev_vals[count - i - 1 + addMinusSign] = c;
    }
    return rev_vals;
}

// converts n to base 10
char *itoa(unsigned long n) {
    return _itoa(n, 1, 10);
}

// outputs a string, str, and padds it to width using paddingChar if strlen(str) < width
char *outstr(char *str, int width, char paddingChar) {
    static char buf[STRING_BUFFER_SIZE];
    if (strlen(str) > width) {
        return str;
    }
    int space = width - strlen(str);

    for (int i = 0; i < space; i++) {
        buf[i] = paddingChar;
    }
    for (int i = 0; i < strlen(str); i ++) {
        buf[space + i] = str[i];
    }
    buf[space + strlen(str)] = '\0';
    return buf;
}

void kprintf(char *fmt, ...) {
    va_list ap;
    int d;
    char c, *s;
    int prev = 0, // specifies if the previous char was a control character '%'
        isSigned = 1 ; // Indicates that the following integer type b, d, o, or x should be treated as an unsigned value 
    
    char widthDigits[STRING_BUFFER_SIZE]; // for storing the digits to specify the width of a string to be printed
    int digitLocation = 0; // for indexing into widthDigits
    for (int i = 0; i < STRING_BUFFER_SIZE; i++) { // reset the values in the array to 0
        widthDigits[i] = 0;
    }

    va_start(ap, fmt);
    while (*fmt) {
        char chr = *fmt;
        *fmt++;
        if (chr == '%') {              /* control */
            if (prev) {
                putchar('%');
                prev = 0;
                continue;
            }
            prev = 1;
            isSigned = 1; // treat all numbers as signed unless otherwise told
            digitLocation = 0; // reset to start of the array
            continue;
        }
        if (chr == 's') {              /* string */
            if (prev) {
                s = va_arg(ap, char *);
                s = outstr(s, strToInt(widthDigits), ' ');
                prev = 0;
                while (*s) {
                    putchar (*s);
                    s++;
                }
                continue;
            }
        }
        if (chr == 'd') {            /* int */
            if (prev) {
                d = va_arg(ap, int);
                prev = 0;
                // printf("Digits: %s. -- %d\n", widthDigits,  strToInt(widthDigits));
                // status("Recvd digits: %d", d);
                char *s = _itoa(d, isSigned, 10);
                s = outstr(s, strToInt(widthDigits), '0');
                // printf("a: '%s'\n", outstr(s, strToInt(widthDigits), '0'));
                while (*s) {
                    putchar(*s);
                    s++;
                }
                continue;
            }
        }
        if (chr == 'b') {            /* binary */
            if (prev) {
                d = va_arg(ap, int);
                prev = 0;
                char *s = _itoa(d, isSigned, 2);
                s = outstr(s, strToInt(widthDigits), ' ');

                while (*s) {
                    putchar(*s);
                    s++;
                }
                continue;
            }
        }
        if (chr == 'o') {            /* octal */
            if (prev) {
                d = va_arg(ap, int);
                prev = 0;
                char *s = _itoa(d, isSigned, 8);
                s = outstr(s, strToInt(widthDigits), ' ');

                while (*s) {
                    putchar(*s);
                    s++;
                }
                continue;
            }
        }
        if (chr == 'x') {            /* hex */
            if (prev) {
                d = va_arg(ap, int);
                prev = 0;
                char *s = _itoa(d, isSigned, 16);
                s = outstr(s, strToInt(widthDigits), ' ');

                while (*s) {
                    putchar(*s);
                    s++;
                }
                continue;
            }
        }
        if (chr == 'c') {              /* char */
            /* need a cast here since va_arg only
            takes fully promoted types */
            if (prev) {
                c = (char) va_arg(ap, int);
                prev = 0;
                putchar(c);
                continue;

            }
        }

        if (chr == 'u') {              /* unsigned */
            if (prev) {
                isSigned = 0; // set as unsigned - not the default
                continue;
            }
        }

        if (chr >= '0' && chr <= '9') {
            if (prev) {
                widthDigits[digitLocation] = chr;
                digitLocation++;

                widthDigits[digitLocation] = '\0';
                continue;
            }
        }

        putchar(chr);
    }
    va_end(ap);
}