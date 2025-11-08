#include <drivers/ps2/keyboard/keyboard.h>
#include <stdio.h>
#include <io/vga.h>

uint32_t getStr(char *addr, uint32_t maxLen, char ascii) {
    // get chars until \n pressed and place into addr with max len = maxLen-1
    uint8_t c;
    uint32_t pos = 0;
    while ((c = getChar()) != '\n') {
        switch (c) {
            case '\b':
                // backspace
                if (pos > 0) {
                    addr[--pos] = 0; // go back a position and  overwrite what was in that position
                    break;
                }
                else
                    continue;
            default:
                // if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))) {
                //     // not a valid character
                //     status("%c %d is not a valid char a-z, A-Z", c, c);
                //     continue;
                // }
                if (ascii)
                    if (c > 128 || c < ' ')
                        if (c != '\t')
                            break; // if char is above 128 then not an ascii character so return
     
                if (pos >= maxLen - 1) {
                    lStatus("Input too long max %d", maxLen -1);
                    continue;
                }
                addr[pos++] = c;
                break;
            }
           int charsPrinted = kprintf("%c", c);
           if (charsPrinted != 1)
               if (charsPrinted > 1)
                   pos+= charsPrinted-1;
           rStatus("POS = %d, c = %c -> %d", pos,c, c);
        }
    // return the number of characters saved in addr 
    return pos;
}
