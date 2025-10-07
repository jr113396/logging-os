#include <stdarg.h>
#include "kprintf.h"
#include "vga.h"
#include "io.h"
#include "util/conversions.h"
#include "globals.h"
#include "mem/stdmem.h"

static int x = 0;
static int y = 0;

//
//   Move cursor to position x,y
//   - Notice this uses isolated I/O
//
void setcursor (int _x, int _y) {
   uint16_t posn;
   y = _y;
   x = _x;

   posn = _y * linelen + _x;
   outb (ctrlreg, 14);
   outb (datareg, posn >> 8);
   outb (ctrlreg, 15);
   outb (datareg, posn &  0xff);
}


void clearScreen() {
   uint16_t * screenp = videoram;
   for (int y = 0; y < lines; y++)
      for (int x = 0; x < linelen; x++)
         *screenp++ = ' ' | BACKGROUND(terminal_bg_colour);

}

char getCharAtPos(int xPos, int yPos) {
    uint16_t * screenp = (videoram) + (linelen * yPos) + (xPos);
   
    // draw char at pos with given values
    char c = *screenp;
    return c & 0xFF;
}

// Write an error
// currently writes to status, may change
void writeError(char *error) {
   status(error);
}

void scrollScreen() {
    // scroll the first 23 lines of the screen up 1 line cutting off the first and clearing the 23rd
    for (int line = 1; line < (lines); line++) {
        for (int xPos = 0; xPos < linelen; xPos++) {
            uint16_t *currentLine = (videoram) + (linelen * line) + (xPos); // current line
            uint16_t *prevLine = (videoram) + (linelen * (line -1)) + (xPos); // prev line
            *prevLine = *currentLine; // move the data from the current line up one line position
            if (line == lines -1) {
                *currentLine = ' ' | BACKGROUND(terminal_bg_colour);
            }
        }
    }
    // for (int xPos = 0; xPos < linelen; xPos++) {
    //     uint16_t *screenPos = (videoram) + (linelen * (lines-1)) + xPos;
    //     *screenPos = '@' | BACKGROUND(terminal_bg_colour);
    // }
}


void terminalGotoPos(int _x, int _y) {
    if (_x < 0 || _x > linelen) {
        // status("X provided %d is outside of the bounds 0 - %d", _x, linelen);
        return;
    }
    if (_y < 0 || _y > lines - 1) {
        // status("Y provided %d is outside of the bounds 0 - %d", _y, lines-1);
        return;
    }
    x = _x;
    y = _y;
    setcursor(x, y);

}

int terminalGetX() {
    return x;
}
int terminalGetY() {
    return y;
}

void putcharcol (char c, int fgColour, int bgColour) {
    //
    //   Store character value (with colour information) at memory location
    //   of current x,y screen position and update x and y ready to move
    //   the cursor.
    
    // check if the entered foreground colour is invalid
    if (fgColour < BLACK || fgColour > WHITE) {
        lStatus("Invalid foreground colour entered: %d", fgColour);
        return;
    }
    
    if (bgColour < BLACK || bgColour > WHITE) {
        lStatus("Invalid background colour entered: %d", bgColour);
        return;
    }
    int newX = x;
    int newY = y;
    int controlChar = 0;
    int foundSpace = 0;
    //  status("CTRL Char %d - %d, %d, %d", c, '\n', '\r', '\0');
    switch (c) {
        case '\n':
            newX = 0; // move to the start of the line
            newY++; // move to the next line 
            controlChar = 1;
            break;

        case '\r':
            newX = 0; // move to the start of the line
            controlChar = 1;
            break;
        
        case '\0':
            controlChar = 1;   
            break;
        
        case '\b':
            controlChar = 1;
            foundSpace = 0;
            // if at top left - do nothing 
            if (newX == 0 && newY == 0) {
                break;
            }

            // if at the start of a line, go back to up a line and to the end
            if (newX == 0) {
                newY--; // go up a line 

                for (int i = linelen-1; i >= 0; i--) {
                    char c = getCharAtPos(i, newY);
                    if (c == ' ') {
                        foundSpace++;
                        continue;
                    }
                    else
                        break;
                }
                if (foundSpace > 2)
                    newX = (linelen - foundSpace);
                else
                    newX = linelen-1; // and to the end of the line
                
                putCharAtPos(' ', newX, newY, fgColour, terminal_bg_colour);
                break;
            }

            // if in middle of line, go left and write over
            newX--;
            putCharAtPos(' ', newX, newY, fgColour, terminal_bg_colour);
            break; 

        
        case '\t':
            controlChar = 1;
            // go to next [tabspace] alligned column
            lStatus("At x %d, next tab at space %d -> %d", newX, newX + tabspace - (newX % tabspace), (newX % tabspace));
            for (int i = 0; i < (tabspace - (newX % tabspace) - 1); i++) {
                putchar(' ');
            }
            newX = x;
            break;

        default:
            controlChar = 0;
            break;
    }
    
    if (newY > (lines - 1)) {// 1 line reserved for status
        // newY = 0; // wrap to top - need to fix by shunting everything up
        scrollScreen();
        newY--;
    }
    if (!controlChar) {
        //   status("Placed %c at x: %d, y: %d",c, x, y);
        putCharAtPos(c, x, y, fgColour, bgColour);
        newX++;
    }
    if (newX >= linelen) {
        newX = 0;
        newY++;
    }
    if (newY > (lines - 1)) {// 1 line reserved for status
        // newY = 0; // wrap to top - need to fix by shunting everything up
        scrollScreen();
        newY--;
    }
    // status("Current y%d - maxY %d", newY, (lines -1));
    x = newX;
    y = newY;
    setcursor(x, y);
}

void putchar (char c) {
    putcharcol(c, WHITE, BLUE);
}

//
//   Clear the screen and move cursor to position 0,0
//
void vgainit ( ) {
    uint16_t * screenp = videoram;
    
    // 
    //   The following just shows how you can output three characters
    //
    //
    //   Clear the screen
    //

    x = 0; y = 0;
    setcursor(x, y);
}


void putCharAtPos(char c, int xPos, int yPos, size_t fgCol, size_t bgCol) {
   // Put a character on the screen at a specific position
   // c - char to write
   // fgCol - foreground colour of the char
   // bgCol - background colour of the char
   // xPos  - xPos to write to 
   // yPos  - yPos to write to
   //
   // **Errors**
   // if y > lines or y < 0 - 'y specifed outside of allowed lines' written to error
   // if x > linelen or x < 0 - 'x specifed outside of allowed line len' written to error
   //
   // **NOTE**
   // does not check status area - can write to whole screen


   // check that the x and y pos are on the drawable area of the screen
   if (yPos > lines || yPos < 0) {
      // writeError("y specifed outside of allowed lines");  
      return;
   }
   if (xPos > linelen || xPos < 0) {
      // writeError("specifed outside of allowed line len");
      return;
   }

   // go to the specified position on the screen 
   uint16_t * screenp = (videoram) + (linelen * yPos) + (xPos);
   
   // draw char at pos with given values
   *screenp = c | FOREGROUND(fgCol) | BACKGROUND(bgCol);
}

void status(char *fmt, ...) {
   va_list ap;
   
   char clearStatus = 1; // clear the status line before writing a new one

   if (clearStatus) {
      for (int i = 0; i < linelen; i++) {
         *(videoram + (linelen * lines) + (i)) = ' ';
      }
   }

    int d;
    char c, *s;
    int prev = 0, // specifies if the previous char was a control character '%'
        isSigned = 1 ; // Indicates that the following integer type b, d, o, or x should be treated as an unsigned value 
    
    char widthDigits[STRING_BUFFER_SIZE]; // for storing the digits to specify the width of a string to be printed
    int digitLocation = 0; // for indexing into widthDigits
    for (int i = 0; i < STRING_BUFFER_SIZE; i++) { // reset the values in the array to 0
        widthDigits[i] = 0;
    }

   if (!fmt) return;

   int originalX = x;
   int originalY = y;

   x = 0;
   y = lines;

   uint16_t * screenp = (videoram) + (linelen * y) + (x);

    va_start(ap, fmt);
    while (*fmt) {
        char chr = *fmt;
        *fmt++;
        if (chr == '%') {              /* control */
            if (prev) {
               *screenp++ = '%' | FOREGROUND(BLACK) | BACKGROUND(WHITE);

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
                  *screenp++ = *s | FOREGROUND(BLACK) | BACKGROUND(WHITE);
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

                char *s = _itoa(d, isSigned, 10);
                s = outstr(s, strToInt(widthDigits), '0');
                // printf("a: '%s'\n", outstr(s, strToInt(widthDigits), '0'));
                while (*s) {
                  *screenp++ = *s | FOREGROUND(BLACK) | BACKGROUND(WHITE);
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
                  *screenp++ = *s | FOREGROUND(BLACK) | BACKGROUND(WHITE);
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
                  *screenp++ = *s | FOREGROUND(BLACK) | BACKGROUND(WHITE);
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
                  *screenp++ = *s | FOREGROUND(BLACK) | BACKGROUND(WHITE);
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
               *screenp++ = c | FOREGROUND(BLACK) | BACKGROUND(WHITE);
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

      *screenp++ = chr | FOREGROUND(BLACK) | BACKGROUND(WHITE);
    }

   x = originalX;
   y = originalY;
   setcursor (x, y);
   va_end(ap);
}


void lStatus(char *fmt, ...) {
   va_list ap;
    uint32_t    leftBgCol = WHITE,
                leftFgCol = GREEN;
   char clearStatus = 1; // clear the status line before writing a new one
   int maxNoDigits = statusBreak - 1;
   if (clearStatus) {
      for (int i = 0; i < maxNoDigits; i++) {
         *(videoram + (linelen * lines) + (i)) = ' ';
      }
   }

    int d;
    char c, *s;
    int prev = 0, // specifies if the previous char was a control character '%'
        isSigned = 1 ; // Indicates that the following integer type b, d, o, or x should be treated as an unsigned value 
    
    char widthDigits[STRING_BUFFER_SIZE]; // for storing the digits to specify the width of a string to be printed
    int digitLocation = 0; // for indexing into widthDigits
    for (int i = 0; i < STRING_BUFFER_SIZE; i++) { // reset the values in the array to 0
        widthDigits[i] = 0;
    }

    if (!fmt) return;

    int originalX = x;
    int originalY = y;

    x = 0;
    y = lines;

    uint16_t *screenp = (videoram) + (linelen * y) + (x);
    uint16_t noDigitsPrinted = 0;
    va_start(ap, fmt);
    while (*fmt) {
        char chr = *fmt;
        *fmt++;
        if (chr == '%') {              /* control */
            if (prev) {
               *screenp++ = '%' | FOREGROUND(leftFgCol) | BACKGROUND(leftBgCol);

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
                  *screenp++ = *s | FOREGROUND(leftFgCol) | BACKGROUND(leftBgCol);
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

                char *s = _itoa(d, isSigned, 10);
                s = outstr(s, strToInt(widthDigits), '0');
                // printf("a: '%s'\n", outstr(s, strToInt(widthDigits), '0'));
                while (*s) {
                  *screenp++ = *s | FOREGROUND(leftFgCol) | BACKGROUND(leftBgCol);
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
                  *screenp++ = *s | FOREGROUND(leftFgCol) | BACKGROUND(leftBgCol);
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
                  *screenp++ = *s | FOREGROUND(leftFgCol) | BACKGROUND(leftBgCol);
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
                  *screenp++ = *s | FOREGROUND(leftFgCol) | BACKGROUND(leftBgCol);
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
               *screenp++ = c | FOREGROUND(leftFgCol) | BACKGROUND(leftBgCol);
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

        *screenp++ = chr | FOREGROUND(leftFgCol) | BACKGROUND(leftBgCol);
        if (++noDigitsPrinted > maxNoDigits) {
            // printed more chars than space
            // so break and dont print the rest
            break;
        }
    }

   x = originalX;
   y = originalY;
   setcursor (x, y);
   va_end(ap);
}

void rStatus(char *fmt, ...) {
   va_list ap;
   uint32_t rightBgCol = BLACK,
            rightFgCol = RED;

//    uint16_t buffer[linelen - statusBreak];
   char clearStatus = 1; // clear the status line before writing a new one
   int maxNoDigits = linelen - statusBreak;
   if (clearStatus) {
      for (int i = 0; i < maxNoDigits; i++) {
         *(videoram + (linelen * lines) + (i) + statusBreak) = ' ';
      }
   }

   uint16_t buffer[maxNoDigits]; // make a buffer to write chars into

    int d;
    char c, *s;
    int prev = 0, // specifies if the previous char was a control character '%'
        isSigned = 1 ; // Indicates that the following integer type b, d, o, or x should be treated as an unsigned value 
    
    char widthDigits[STRING_BUFFER_SIZE]; // for storing the digits to specify the width of a string to be printed
    int digitLocation = 0; // for indexing into widthDigits
    for (int i = 0; i < STRING_BUFFER_SIZE; i++) { // reset the values in the array to 0
        widthDigits[i] = 0;
    }

    if (!fmt) return;

    int originalX = x;
    int originalY = y;

    x = 0;
    y = lines;

    uint16_t *screenp = buffer;//(videoram) + (linelen * y) + (x) + (statusBreak);
    uint16_t noDigitsPrinted = 0;
    va_start(ap, fmt);
    while (*fmt) {
        char chr = *fmt;
        *fmt++;
        if (chr == '%') {              /* control */
            if (prev) {
                *screenp++ = *s++ | FOREGROUND(rightFgCol) | BACKGROUND(rightBgCol);
                if (++noDigitsPrinted > maxNoDigits) {
                    // printed more chars than space
                    // so break and dont print the rest
                    break;
                }
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
                    *screenp++ = *s++ | FOREGROUND(rightFgCol) | BACKGROUND(rightBgCol);
                    if (++noDigitsPrinted > maxNoDigits) {
                        // printed more chars than space
                        // so break and dont print the rest
                        break;
                    }
                }
                continue;
            }
        }
        if (chr == 'd') {            /* int */
            if (prev) {
                d = va_arg(ap, int);
                prev = 0;
                // printf("Digits: %s. -- %d\n", widthDigits,  strToInt(widthDigits));

                char *s = _itoa(d, isSigned, 10);
                s = outstr(s, strToInt(widthDigits), '0');
                // printf("a: '%s'\n", outstr(s, strToInt(widthDigits), '0'));
                while (*s) {
                    *screenp++ = *s++ | FOREGROUND(rightFgCol) | BACKGROUND(rightBgCol);
                    if (++noDigitsPrinted > maxNoDigits) {
                        // printed more chars than space
                        // so break and dont print the rest
                        break;
                    }
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
                    *screenp++ = *s++ | FOREGROUND(rightFgCol) | BACKGROUND(rightBgCol);
                    if (++noDigitsPrinted > maxNoDigits) {
                        // printed more chars than space
                        // so break and dont print the rest
                        break;
                    }
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
                    *screenp++ = *s++ | FOREGROUND(rightFgCol) | BACKGROUND(rightBgCol);
                    if (++noDigitsPrinted > maxNoDigits) {
                        // printed more chars than space
                        // so break and dont print the rest
                        break;
                    }
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
                    *screenp++ = *s++ | FOREGROUND(rightFgCol) | BACKGROUND(rightBgCol);
                    if (++noDigitsPrinted > maxNoDigits) {
                        // printed more chars than space
                        // so break and dont print the rest
                        break;
                    }

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
                *screenp++ = c | FOREGROUND(rightFgCol) | BACKGROUND(rightBgCol);
                if (++noDigitsPrinted > maxNoDigits) {
                    // printed more chars than space
                    // so break and dont print the rest
                    break;
                }
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

        *screenp++ = chr | FOREGROUND(rightFgCol) | BACKGROUND(rightBgCol);
        if (++noDigitsPrinted > maxNoDigits) {
            // printed more chars than space
            // so break and dont print the rest
            break;
        }
    }

    
    // now got all the values in buffer
    // so right allign and memcpy
    // outstr(buffer, maxNoDigits, ' ');
    memcpy((videoram) + (linelen * y) + (x) + (statusBreak) + (maxNoDigits - noDigitsPrinted) , buffer, maxNoDigits);

    x = originalX;
    y = originalY;
    setcursor (x, y);
    va_end(ap);
}