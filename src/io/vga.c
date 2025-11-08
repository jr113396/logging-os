#include "kprintf.h"
#include "vga.h"
#include "io.h"
#include <globals.h>
#include <stdarg.h>
#include <util/conversions.h>
#include <mem/stdmem.h>
#include <stdio.h>

static int x = 0;
static int y = 0;
static int statusX = 0;

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

void putStatusChar(char c) {
    if (statusX > linelen) {
        statusX = 0;
    }
    putCharAtPos(c, statusX, lines, BLACK, WHITE);
    statusX++;
}


void rstStatus(int start, int end) {
    if (start < 0 || end > linelen) {
        return;
    }
    for (;start < end; start++) {
        putCharAtPos(' ', start, lines, WHITE, terminal_bg_colour);
    }
}

int status(char *fmt, ...) {
    if (!fmt) return 0;
    va_list ap;

    char clearStatus = 1; // clear the status line before writing a new one

    if (clearStatus) {
        rstStatus(0, linelen);
    }
    statusX = 0;

    va_start(ap, fmt);

    int numChars = kfprintf(putStatusChar, fmt, ap);

    va_end(ap);
    return numChars;
}

void putLStatusChar(char c) {
    if (statusX > statusBreak -1) {
        statusX = 0;
    }
    putCharAtPos(c, statusX, lines, WHITE, GREEN);
    statusX++;
}

int lStatus(char *fmt, ...) {
    if (!fmt)  return 0;
    va_list ap;

    statusX = 0;
    char clearStatus = 1; // clear the status line before writing a new one

    if (clearStatus) {
        rstStatus(0, statusBreak -1);
    }
    va_start(ap, fmt);

    int numChars = kfprintf(putLStatusChar, fmt, ap);

    va_end(ap);
    return numChars;
}

void putRStatusChar(char c) {
    if (statusX > linelen) {
        statusX = statusBreak;
    }
    putCharAtPos(c, statusX, lines, RED, BLACK);
    statusX++;
}


int rStatus(char *fmt, ...) {
    if (!fmt) return 0;
    va_list ap;

    statusX = statusBreak;
    char clearStatus = 1; // clear the status line before writing a new one

    if (clearStatus) {
        rstStatus(statusBreak, linelen);
    }
    va_start(ap, fmt);

    int numChars = kfprintf(putRStatusChar, fmt, ap);

    va_end(ap);
    return numChars;
}
