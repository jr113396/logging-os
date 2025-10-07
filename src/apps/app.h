#ifndef apph
#define apph

#include "stdint.h"

struct appStruct {
    char **names; // the names of the function
    uint8_t noNames; // the number names for this app
    char *help; // the help message printed when "help" is used
    void *func; // a ptr to the init function of the app
    uint8_t rstAfter; // do you reset the terminal after returning, 0 for no,  not 0 for yes
};

#endif