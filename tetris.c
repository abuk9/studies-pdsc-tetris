#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include "primlib.h"
#include "pieces.h"

#define FRAMETIME 0.8 // In seconds
#define GAME_HEIGHT 20
#define GAME_WIDTH 15

#define PIXEL_SIZE 30
#define BORDER 3
#define MARGIN 3
#define BOARD_HEIGHT GAME_HEIGHT*(PIXEL_SIZE + MARGIN) - MARGIN
#define BOARD_WIDTH GAME_WIDTH*(PIXEL_SIZE + MARGIN) - MARGIN

char board[GAME_HEIGHT][GAME_WIDTH] = {0};
int boardCoors[2];
typedef struct {
    int coors[2];
    int kind;
    int rotation;
} piece;

void initVariables() {
    boardCoors[0] = (gfx_screenWidth() - BOARD_WIDTH) / 2;
    boardCoors[1] = (gfx_screenHeight() - BOARD_HEIGHT) / 2;
    for (int x = 0; x < GAME_WIDTH; x++) board[19][x] = 1;
    for (int x = 0; x < GAME_WIDTH; x++) board[18][x] = 1;
    board[18][0] = 0;
}

void drawPixel(int x, int y, int color) {
    int xCoor = boardCoors[0] + x*(PIXEL_SIZE + MARGIN);
    int yCoor = boardCoors[1] + y*(PIXEL_SIZE + MARGIN);
    gfx_filledRect(xCoor, yCoor, xCoor + PIXEL_SIZE -1,
        yCoor + PIXEL_SIZE -1, color);
}

void drawFrame() {
    for (int i = 0; i < 2; i++) {
        int color = i == 0 ? RED : BLACK;
        int off = i == 0 ? BORDER + MARGIN : MARGIN;
        gfx_filledRect(boardCoors[0] - off, boardCoors[1] - off,
            boardCoors[0] + off + BOARD_WIDTH,
            boardCoors[1] + off + BOARD_HEIGHT, color);
    }
}

void drawBoard() {
    gfx_filledRect(0, 0, gfx_screenHeight()-1, gfx_screenHeight()-1, BLACK);
    drawFrame();
    for (int y = 0; y < GAME_HEIGHT; y++) {
        for (int x = 0; x < GAME_WIDTH; x++) {
            if (board[y][x]) drawPixel(x, y, MAGENTA); 
        }
    }
}

void drawPiece(piece tile) {
    const char (*shape)[4] = pieces[tile.kind][tile.rotation];
    for (int dy = 0; dy < 4; dy++) {
        for (int dx = 0; dx < 4; dx++) {
            // ASK QUESTION ABOUT THIS
            char isFilled = *(*(shape+dy)+dx);
            if (isFilled) {
                int color = isFilled == 1 ? CYAN : BLUE;
                drawPixel(tile.coors[0] + dx, tile.coors[1] + dy, color);
            }
        }
    }
}

void lockPiece(piece tile) {
    const char (*shape)[4] = pieces[tile.kind][tile.rotation];
    for (int dy = 0; dy < 4; dy++) {
        for (int dx = 0; dx < 4; dx++) {
            char isFilled = *(*(shape+dy)+dx);
            if (isFilled) {
                int x = tile.coors[0] + dx;
                int y = tile.coors[1] + dy;
                board[y][x] = 1;
            }
        }
    }
}

void collapseBoard() {
    // Why doesn't this work?
    // memcpy(board[1], board[0], sizeof(char)*(GAME_HEIGHT-1)*GAME_WIDTH);
    for (int y = GAME_HEIGHT-1; y >= 1; y--) {
        memcpy(board[y], board[y-1], sizeof(char)*GAME_WIDTH);
    }
    for (int x = 0; x < GAME_WIDTH; x++) board[0][x] = 0;
}

void checkConstants() {
    int totalHeight = BOARD_HEIGHT + 2*(MARGIN+BORDER);
    int totalWidth = BOARD_WIDTH + 2*(MARGIN+BORDER);
    assert(totalHeight <= gfx_screenHeight());
    assert(totalWidth <= gfx_screenWidth());
}

bool isGameLost() {
    for (int x=0; x < GAME_WIDTH; x++) {
        if (board[0][x]) return true;
    }
    return false;
}

piece initPiece() {
    piece thePiece;
    thePiece.kind = rand() % PIECE_KINDS;
    assert(thePiece.kind < 8);
    thePiece.coors[0] = GAME_WIDTH / 2 - 2;
    thePiece.coors[1] = 0;
    thePiece.rotation = 0;
    return thePiece;
}

bool hasPieceFallen(piece thePiece) {
    const char (*shape)[4] = pieces[thePiece.kind][thePiece.rotation];
    for (int dy = 0; dy < 4; dy++) {
        for (int dx = 0; dx < 4; dx++) {
            char isFilled = *(*(shape+dy)+dx);
            if (isFilled) {
                int x = thePiece.coors[0] + dx;
                int y = thePiece.coors[1] + dy;
                if (y == GAME_HEIGHT -1) return true;
                if (board[y+1][x]) return true;
            }
        }
    }
    return false;
}

void render(piece thePiece) {
    drawBoard();
    drawPiece(thePiece);
    gfx_updateScreen();
}

void handleAction(int key, piece* thePiece) {}

void run() {
    while (!isGameLost()) {
        piece thePiece = initPiece();
        while (!hasPieceFallen(thePiece)) {
            thePiece.coors[1]++;
            render(thePiece);
            long start = clock();
            while((clock() - start)/CLOCKS_PER_SEC < FRAMETIME) {
                int key = gfx_pollkey();
                if (key != -1) {
                    handleAction(key, &thePiece);
                    render(thePiece);
                }
            }
        }
        lockPiece(thePiece);
    }
}

int main() {
    if (gfx_init()) abort();
    checkConstants();
    initVariables();
    srand((unsigned) time(NULL));
    run();
    return 0;
}
