#ifndef keymaph
#define keymaph

#include "stdint.h"



const uint8_t  UNMAP   = 0xFF - 0;
const uint8_t  ESC     = 0xFF - 1;
const uint8_t  LCTRL   = 0xFF - 2;
const uint8_t  LSHIFT  = 0xFF - 3;
const uint8_t  RSHIFT  = 0xFF - 4;
const uint8_t  ALT     = 0xFF - 5;
const uint8_t  SPACE   = ' ';//0xFF - 6;
const uint8_t  CAPS    = 0xFF - 7;
const uint8_t  F1      = 0xFF - 8;
const uint8_t  F2      = 0xFF - 9;
const uint8_t  F3      = 0xFF - 10;
const uint8_t  F4      = 0xFF - 11;
const uint8_t  F5      = 0xFF - 12;
const uint8_t  F6      = 0xFF - 13;
const uint8_t  F7      = 0xFF - 14;
const uint8_t  F8      = 0xFF - 15;
const uint8_t  F9      = 0xFF - 16;
const uint8_t  F10     = 0xFF - 17;
const uint8_t  F11     = 0xFF - 18;
const uint8_t  F12     = 0xFF - 19;
const uint8_t  NUMLCK  = 0xFF - 20;
const uint8_t  SCRLLCK = 0xFF - 21;
const uint8_t  HOME    = 0xFF - 22;
const uint8_t  UP      = 0xFF - 23;
const uint8_t  DOWN    = 0xFF - 24;
const uint8_t  LEFT    = 0xFF - 25;
const uint8_t  RIGHT   = 0xFF - 26;
const uint8_t  PAGEUP  = 0xFF - 27;
const uint8_t  PAGEDN  = 0xFF - 28;
const uint8_t  END     = 0xFF - 29;
const uint8_t  INS     = 0xFF - 30;
const uint8_t  DEL     = 0xFF - 31;


const uint8_t lowercaseMap[128] = {
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

const uint8_t shiftMap[128] = {
    UNMAP,
    ESC,
    '!',
    '"',
    '3',//'£', -- TO FIX
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
    '`', // - TO FIX'¬',    // 41
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

const uint8_t capsMap[128] = {
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

const uint8_t shiftCapsMap[128] = {
    UNMAP,
    ESC,
    '!',
    '"',
    '3', // '£', - TO FIX
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
    '`', //'¬', - TO FIX    // 41
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