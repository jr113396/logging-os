#include "gameOfLife.h"
#include "io/kprintf.h"
#include "mem/stdmem.h"
#include "io/vga.h"
#include "stdint.h"
#include "pit/pit.h"
#include "drivers/ps2/keyboard/keyboard.h"
#include "drivers/ps2/keyboard/keymap.h"


static uint8_t 
        aliveVal    = 1, 
        deadVal     = 0,
        game_linelen     = 80,
        game_rows        = 23;


void setMapVal(uint8_t *map, uint8_t x, uint8_t y, uint8_t val){
    map[x + game_linelen * y] = val;
}

void gameSetup(uint8_t * currentState) {
    memset(currentState, 0, game_linelen*game_rows);
}

uint8_t isSquareAlive(uint8_t *current, int x, int y) {
    if (x > game_linelen || x < 0 || y > game_rows || y < 0)
        return 0; // dead as out of bounds
    
    uint8_t square = current[x + game_linelen * y];
    return (square) ? 1 : 0;
}

uint8_t getNoAliveneighbours(uint8_t *current, int x, int y) {
    uint8_t noAlive = 0; // number of alive neighbours

    //00000
    //0XXX0 - alive neightbours are in X positions arround @
    //0X@X0
    //0XXX0
    //00000

    // uint8_t onBoundry = 0; // 000LRTB0
    // if (x % game_linelen == 0) // left bound
    //     onBoundry |= 1 << 4;

    // if (x % game_linelen == game_linelen-1) // right bound
    //     onBoundry |= 1 << 3;

    // if (y == 0) // top bound
    //     onBoundry |= 1 << 2;

    // if (y == game_rows - 1) // bottom bound
    //     onBoundry |= 1 << 1;

        // above
    if (!((y == 0) && (x == 0)))// top left 
        noAlive += isSquareAlive(current, x-1, y-1);
    if (!(y == 0))
        noAlive += isSquareAlive(current, x, y-1); // top mid
    if (!((y == 0) && (x == game_linelen-1)))// top right
        noAlive += isSquareAlive(current, x, y+1);
    // right
    if (!(x == game_linelen-1))// mid right
        noAlive += isSquareAlive(current, x+1, y);

    // bellow
    if (!((x == game_linelen-1) && (y == game_rows - 1))) // bottom right
        noAlive += isSquareAlive(current, x+1, y+1);
    if (!(y == game_rows -1))
        noAlive += isSquareAlive(current, x, y+1);
    if (!((y == game_rows -1) && (x == 0)))
        noAlive += isSquareAlive(current, x-1, y+1);

    // left
    if (!(x % game_linelen == 0))
        noAlive += isSquareAlive(current, x-1, y);
    return noAlive;
}

void genNextState(uint8_t *current, uint8_t *next) {
    for (int x = 0; x < game_linelen; x++) {
        for (int y = 0; y < game_rows; y++) {
            uint8_t alive = getNoAliveneighbours(current, x, y);
            if (current[x + game_linelen * y] == aliveVal) { // if currently alive
                if (alive < 2) // dies from underpopulation
                    setMapVal(next, x, y, deadVal);
                else if (alive < 4) // survives
                    setMapVal(next, x, y, aliveVal);

                else // dies from overpopulation
                    setMapVal(next, x, y, deadVal);
            } else { // currently dead
                if (alive == 3) // repopulation
                    lStatus("repopulation %d", alive);
                    setMapVal(next, x, y, aliveVal);
            }
        }
    }
}

void printGameState(uint8_t *current) {
    for (int x = 0; x < game_linelen; x++) {
        for (int y = 0; y < game_rows; y++) {
            if (current[x + game_linelen * y] == aliveVal) {
                    putCharAtPos('@', x, y, BLACK, WHITE);
            }
            else {
                putCharAtPos('X', x, y, WHITE, BLACK);
            }    
        }
    }
}

uint16_t numberAlive(uint8_t *current) {
    uint16_t numAlive = 0;
    for (int x = 0; x < game_linelen; x++)
        for (int y = 0; y < game_rows; y++)
            if (current[x + game_linelen * y] == aliveVal)
                numAlive++;
    return numAlive;
}

void initGameOfLife() {
    uint8_t    currentState[game_linelen * game_rows],
            nextState[game_linelen * game_rows];


    gameSetup(currentState);
    uint32_t gen = 0;
    for (int x = 0; x < game_linelen; x++) {
        for (int y = 0; y <= game_rows; y++) {
            if (isKeyPressed(LCTRL) && (isKeyPressed('c') || isKeyPressed('C'))) {
                return;
            }
            gen++;
            printGameState(currentState);
            genNextState(currentState, nextState);
            memcpy(currentState, nextState, game_linelen * game_rows);
            pauseTimeMs(1500);
            rStatus("(Alive: %d) Gen: %d",numberAlive(currentState), gen);
        }
    }
}
