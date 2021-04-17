#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "primlib.h"
#include "pieces.h"
#define BOARD_HEIGHT 20
#define BOARD_WIDTH 15
#define BOARD_BORDER 3
#define MARGIN 2
#define PIXEL_SIZE 30

char board[BOARD_HEIGHT][BOARD_WIDTH] = {{1}};
int boardCoors[2][2];

void initVariables() {
    boardCoors[0][0] = (gfx_screenWidth() - BOARD_WIDTH*(PIXEL_SIZE+MARGIN)) / 2;
    boardCoors[0][1] = (gfx_screenHeight() - BOARD_HEIGHT*(PIXEL_SIZE+MARGIN)) / 2;
    boardCoors[1][0] = (gfx_screenWidth() + BOARD_WIDTH*(PIXEL_SIZE+MARGIN)) / 2;
    boardCoors[1][1] = (gfx_screenHeight() + BOARD_HEIGHT*(PIXEL_SIZE+MARGIN)) / 2;
    for (int i = 0; i < BOARD_WIDTH; i++) {
        for (int y = 0; y < BOARD_HEIGHT; y++) {
            board[y][i] = 1;
        }
    }
}

void drawPixel(int x, int y, int color) {
    int xCoor = boardCoors[0][0] + x*(PIXEL_SIZE + MARGIN);
    int yCoor = boardCoors[0][1] + y*(PIXEL_SIZE + MARGIN);
    gfx_filledRect(xCoor, yCoor, xCoor + PIXEL_SIZE -1,
        yCoor + PIXEL_SIZE -1, color);
}

void renderBoard() {
    gfx_filledRect(0, 0, gfx_screenHeight()-1, gfx_screenHeight()-1, BLACK);
    for (int i = 0; i < 2; i++) {
        int color = i == 0 ? RED : BLACK;
        int off = i == 0 ? BOARD_BORDER + MARGIN : MARGIN;
        gfx_filledRect(boardCoors[0][0] - off, boardCoors[0][1] - off,
        boardCoors[1][0] + off, boardCoors[1][1] + off, color);
    }

    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (board[y][x]) drawPixel(x, y, MAGENTA); 
        }
    }

    gfx_updateScreen();
}

void checkConstants() {
    int totalHeight = BOARD_HEIGHT*(PIXEL_SIZE+MARGIN)+2*(MARGIN+BOARD_BORDER);
    int totalWidth = BOARD_WIDTH*(PIXEL_SIZE+MARGIN)+2*(MARGIN+BOARD_BORDER);
    assert(totalHeight <= gfx_screenHeight());
    assert(totalWidth <= gfx_screenWidth());
}

int main() {
    if (gfx_init()) abort();
    checkConstants();
    initVariables();
    renderBoard();
    gfx_getkey();
    return 0;
}
