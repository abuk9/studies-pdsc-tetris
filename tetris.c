#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "primlib.h"
#include "pieces.h"
#define BOARD_HEIGHT 20
#define BOARD_WIDTH 15
#define BOARD_BORDER 3
#define PIXEL_SIZE 30

char board[BOARD_HEIGHT][BOARD_WIDTH] = {0};
int boardCoors[2][2];

void initVariables() {
    boardCoors[0][0] = (gfx_screenWidth() - BOARD_WIDTH*PIXEL_SIZE) / 2;
    boardCoors[0][1] = (gfx_screenHeight() - BOARD_HEIGHT*PIXEL_SIZE) / 2;
    boardCoors[1][0] = (gfx_screenWidth() + BOARD_WIDTH*PIXEL_SIZE) / 2;
    boardCoors[1][1] = (gfx_screenHeight() + BOARD_HEIGHT*PIXEL_SIZE) / 2;
}

void renderBoard() {
    gfx_filledRect(0, 0, gfx_screenHeight()-1, gfx_screenHeight()-1, BLACK);
    gfx_filledRect(boardCoors[0][0] - BOARD_BORDER, boardCoors[0][1] - BOARD_BORDER,
    boardCoors[1][0] + BOARD_BORDER, boardCoors[1][1] + BOARD_BORDER, RED);
    gfx_filledRect(boardCoors[0][0]+1, boardCoors[0][1], boardCoors[1][0],
    boardCoors[1][1], BLACK);

    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (board[y][x]) {
                int xCoor = boardCoors[0][0] + x*PIXEL_SIZE;
                int yCoor = boardCoors[0][1] + y*PIXEL_SIZE;
                gfx_filledRect(xCoor, yCoor, xCoor + PIXEL_SIZE -1,
                    yCoor + PIXEL_SIZE -1, MAGENTA);
            } 
        }
    }
    gfx_updateScreen();
}

void checkConstants() {
    assert(BOARD_HEIGHT*PIXEL_SIZE <= gfx_screenHeight());
    assert(BOARD_WIDTH*PIXEL_SIZE <= gfx_screenWidth());
}

int main() {
    if (gfx_init()) abort();
    checkConstants();
    initVariables();
    renderBoard();
    gfx_getkey();
    return 0;
}
