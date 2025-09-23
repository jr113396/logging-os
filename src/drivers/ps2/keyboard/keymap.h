#ifndef keymaph
#define keymaph

#include "stdint.h"



const uint32_t  UNMAP   = 0xFFFFFFFF - 0;
const uint32_t  ESC     = 0xFFFFFFFF - 1;
const uint32_t  LCTRL   = 0xFFFFFFFF - 2;
const uint32_t  LSHIFT  = 0xFFFFFFFF - 3;
const uint32_t  RSHIFT  = 0xFFFFFFFF - 4;
const uint32_t  ALT     = 0xFFFFFFFF - 5;
const uint32_t  SPACE   = 0xFFFFFFFF - 6;
const uint32_t  CAPS    = 0xFFFFFFFF - 7;
const uint32_t  F1      = 0xFFFFFFFF - 8;
const uint32_t  F2      = 0xFFFFFFFF - 9;
const uint32_t  F3      = 0xFFFFFFFF - 10;
const uint32_t  F4      = 0xFFFFFFFF - 11;
const uint32_t  F5      = 0xFFFFFFFF - 12;
const uint32_t  F6      = 0xFFFFFFFF - 13;
const uint32_t  F7      = 0xFFFFFFFF - 14;
const uint32_t  F8      = 0xFFFFFFFF - 15;
const uint32_t  F9      = 0xFFFFFFFF - 16;
const uint32_t  F10     = 0xFFFFFFFF - 17;
const uint32_t  F11     = 0xFFFFFFFF - 18;
const uint32_t  F12     = 0xFFFFFFFF - 19;
const uint32_t  NUMLCK  = 0xFFFFFFFF - 20;
const uint32_t  SCRLLCK = 0xFFFFFFFF - 21;
const uint32_t  HOME    = 0xFFFFFFFF - 22;
const uint32_t  UP      = 0xFFFFFFFF - 23;
const uint32_t  DOWN    = 0xFFFFFFFF - 24;
const uint32_t  LEFT    = 0xFFFFFFFF - 25;
const uint32_t  RIGHT   = 0xFFFFFFFF - 26;
const uint32_t  PAGEUP  = 0xFFFFFFFF - 27;
const uint32_t  PAGEDN  = 0xFFFFFFFF - 28;
const uint32_t  END     = 0xFFFFFFFF - 29;
const uint32_t  INS     = 0xFFFFFFFF - 30;
const uint32_t  DEL     = 0xFFFFFFFF - 31;


const uint32_t lowercaseMap[128] = {
    UNMAP,
    ESC,
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    '0',
    '-',
    '=', // 13
    '\b',
    '\t',
    'q',
    'w',
    'e',
    'r',
    't',
    'y',
    'u',
    'i',
    'o',
    'p',
    '[',
    ']',
    '\n', //28
    LCTRL,
    'a',
    's',
    'd',
    'f',
    'g',
    'h',
    'j',
    'k',
    'l',
    ';',
    '\'',   // 40
    '`',    // 41
    LSHIFT, // 42
    '#',    // 43
    'z',
    'x',
    'c',
    'v',
    'b',
    'n',
    'm',
    ',',
    '.',
    '/',
    RSHIFT,// 54
    '*', // NUM PAD *
    ALT,
    SPACE,
    CAPS,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    NUMLCK,
    SCRLLCK,
    HOME,
    UP,
    PAGEUP,
    '-', // NUM PAD MINUS
    LEFT, // 75
    UNMAP,
    RIGHT, // 77
    '+', // NUM PAD PLUS
    END,
    DOWN, // 80
    PAGEDN,
    INS,
    DEL,
    UNMAP,
    UNMAP,
    '\\',
    F11,
    F12, // 88
    UNMAP,
    UNMAP, // 90
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP, // 100
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP, // 110
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP, // 120
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP
};

const uint32_t shiftMap[128] = {
    UNMAP,
    ESC,
    '!',
    '"',
    '£', 
    '$',
    '%',
    '^',
    '&',
    '*',
    '(',
    ')',
    '_',
    '+', // 13
    '\b',
    '\t',
    'Q',
    'W',
    'E',
    'R',
    'T',
    'Y',
    'U',
    'I',
    'O',
    'P',
    '{',
    '}',
    '\n', //28
    LCTRL,
    'A',
    'S',
    'D',
    'F',
    'G',
    'H',
    'J',
    'K',
    'L',
    ':',
    '@',   // 40
    '¬',    // 41
    LSHIFT, // 42
    '~',    // 43
    'Z',
    'X',
    'C',
    'V',
    'B',
    'N',
    'M',
    '<',
    '>',
    '?',
    RSHIFT,// 54
    '*', // NUM PAD *
    ALT,
    SPACE,
    CAPS,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    NUMLCK,
    SCRLLCK,
    HOME,
    UP,
    PAGEUP,
    '-', // NUM PAD MINUS
    LEFT, // 75
    UNMAP,
    RIGHT, // 77
    '+', // NUM PAD PLUS
    END,
    DOWN, // 80
    PAGEDN,
    INS,
    DEL,
    UNMAP,
    UNMAP,
    '|',
    F11,
    F12, // 88
    UNMAP,
    UNMAP, // 90
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP, // 100
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP, // 110
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP, // 120
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP
};

const uint32_t capsMap[128] = {
    UNMAP,
    ESC,
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    '0',
    '-',
    '=', // 13
    '\b',
    '\t',
    'Q',
    'W',
    'E',
    'R',
    'T',
    'Y',
    'U',
    'I',
    'O',
    'P',
    '[',
    ']',
    '\n', //28
    LCTRL,
    'A',
    'S',
    'D',
    'F',
    'G',
    'H',
    'J',
    'K',
    'L',
    ';',
    '\'',   // 40
    '`',    // 41
    LSHIFT, // 42
    '#',    // 43
    'Z',
    'X',
    'C',
    'V',
    'B',
    'N',
    'M',
    ',',
    '.',
    '/',
    RSHIFT,// 54
    '*', // NUM PAD *
    ALT,
    SPACE,
    CAPS,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    NUMLCK,
    SCRLLCK,
    HOME,
    UP,
    PAGEUP,
    '-', // NUM PAD MINUS
    LEFT, // 75
    UNMAP,
    RIGHT, // 77
    '+', // NUM PAD PLUS
    END,
    DOWN, // 80
    PAGEDN,
    INS,
    DEL,
    UNMAP,
    UNMAP,
    '\\',
    F11,
    F12, // 88
    UNMAP,
    UNMAP, // 90
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP, // 100
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP, // 110
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP, // 120
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP
};

const uint32_t shiftCapsMap[128] = {
    UNMAP,
    ESC,
    '!',
    '"',
    '£',
    '$',
    '%',
    '^',
    '&',
    '*',
    '(',
    ')',
    '_',
    '+', // 13
    '\b',
    '\t',
    'q',
    'w',
    'e',
    'r',
    't',
    'y',
    'u',
    'i',
    'o',
    'p',
    '{',
    '}',
    '\n', //28
    LCTRL,
    'a',
    's',
    'd',
    'f',
    'g',
    'h',
    'j',
    'k',
    'l',
    ':',
    '@',   // 40
    '¬',    // 41
    LSHIFT, // 42
    '~',    // 43
    'z',
    'x',
    'c',
    'v',
    'b',
    'n',
    'm',
    '<',
    '>',
    '?',
    RSHIFT,// 54
    '*', // NUM PAD *
    ALT,
    SPACE,
    CAPS,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    NUMLCK,
    SCRLLCK,
    HOME,
    UP,
    PAGEUP,
    '-', // NUM PAD MINUS
    LEFT, // 75
    UNMAP,
    RIGHT, // 77
    '+', // NUM PAD PLUS
    END,
    DOWN, // 80
    PAGEDN,
    INS,
    DEL,
    UNMAP,
    UNMAP,
    '|',
    F11,
    F12, // 88
    UNMAP,
    UNMAP, // 90
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP, // 100
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP, // 110
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP, // 120
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP,
    UNMAP
};
#endif