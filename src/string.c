#include "string.h"

// returns the number of characters in a null terminated string
// s - null terminated string
int strlen(char *s) {
    char *start = s;
    int i = 0;
    while (*start++) i++;
            // kprintf("%d\n", i);

    return i;
}

int strcmp(char *s1, char *s2) {
    // checks to see if 2 strings are equal,
    // returns 0 if true and the difference between the first differing chars if not
    for (int pos = 0;; pos++) {
        if (s1[pos] != s2[pos]) {
            return s1[pos] - s2[pos];
        }
        if (s1[pos] == '\0')
            break; // if got to the end of strings
    }
    return 0;
}