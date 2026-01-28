#ifndef __loging_level_h
#define __loging_level_h

// #define _SERIAL_LOGGING_DEBUG 1
// #define _SERIAL_LOGGING_INFO 1
// #define _SERIAL_LOGGING_ERROR 1


// #define _PRINT_LOGGING_DEBUG 1
// #define _PRINT_LOGGING_INFO 1
// #define _PRINT_LOGGING_ERROR 1

// used to enable/disable the paging logging system that loggs what was written to pages
// this will log when new pages are created and where, and what data is written
#define PAGING_LOGGING


#define _LOGGING_DEBUG 4
#define _LOGGING_INFO 3
#define _LOGGING_ERROR 2
#define _LOGGING_NONE 1


#define _SERIAL_LOGGING_LEVEL _LOGGING_DEBUG
#define _PRINT_LOGGING_LEVEL _LOGGING_DEBUG

#if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
#endif

#if _SERIAL_LOGGING_LEVEL >= _LOGGING_INFO
#endif

#if _SERIAL_LOGGING_LEVEL >= _LOGGING_ERROR
#endif

#if _SERIAL_LOGGING_LEVEL >= _LOGGING_NONE

#endif
#endif
