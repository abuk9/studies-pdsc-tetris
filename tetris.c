#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include "primlib.h"
#include "pieces.h"

#define FRAMETIME 0.5 // In seconds
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
            char isFilled = *(*(shape+dy)+dx);
            if (isFilled) {
                drawPixel(tile.coors[0] + dx, tile.coors[1] + dy, CYAN);
            }
        }
    }
}

void render(piece thePiece) {
    drawBoard();
    drawPiece(thePiece);
    gfx_filledRect(0, 0, 1, 1, BLACK); // This shouldn't help, but it does -_O.o_-
    gfx_updateScreen();
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

int getFullRow() {
    for (int y = 0; y < GAME_HEIGHT; y++) {
        bool isFull = true;
        for (int x = 0; x < GAME_WIDTH; x++) {
            if (!board[y][x]) {
                isFull = false;
                break;
            }
        }
        if (isFull) return y;
    }
    return -1;
}

void collapseRow(int row) {
    memmove(board[1], board[0], sizeof(char)*GAME_WIDTH*row);
    for (int x = 0; x < GAME_WIDTH; x++) board[0][x] = 0;
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
    thePiece.coors[1] = -1;
    thePiece.rotation = 0;
    return thePiece;
}

bool isPieceCollidingWithBoard(piece aPiece) {
    const char (*shape)[4] = pieces[aPiece.kind][aPiece.rotation];
    for (int dy = 0; dy < 4; dy++) {
        for (int dx = 0; dx < 4; dx++) {
            char isFilled = *(*(shape+dy)+dx);
            if (isFilled) {
                int x = aPiece.coors[0] + dx;
                int y = aPiece.coors[1] + dy;
                if (x < 0 || x >= GAME_WIDTH) return true;
                if (y < 0 || y >= GAME_HEIGHT) return true;
                if (board[y][x]) return true;
            }
        }
    }
    return false;
}

bool hasPieceFallen(piece aPiece) {
    aPiece.coors[1]++;
    return isPieceCollidingWithBoard(aPiece);
}

bool canPieceMove(piece aPiece, int sense) {
    aPiece.coors[0] += sense;
    return !isPieceCollidingWithBoard(aPiece);
}

void findCenter(piece aPiece, int center[2]) {
    const char (*shape)[4] = pieces[aPiece.kind][aPiece.rotation];
    for (int dy = 0; dy < 4; dy++) {
        for (int dx = 0; dx < 4; dx++) {
            char isFilled = *(*(shape+dy)+dx);
            if (isFilled == 2) {
                center[0] = dx;
                center[1] = dy;
                return;
            }
        }
    }
}

void rotatePiece(piece *aPiece) {
    int oldCenter[2], newCenter[2];
    findCenter(*aPiece, oldCenter);
    (*aPiece).rotation = ((*aPiece).rotation + 1) % 4;
    findCenter(*aPiece, newCenter);
    for (int i = 0; i < 2; i++) {
        int vect = oldCenter[i] - newCenter[i];
        (*aPiece).coors[i] += vect;
    }
}

bool canPieceRotate(piece aPiece) {
    rotatePiece(&aPiece);
    return !isPieceCollidingWithBoard(aPiece);
}

void handleAction(int key, piece* thePiece) {
    int sense = 1; // 1 meaning 'right'
    switch (key) {
        case SDLK_ESCAPE: abort();
        case SDLK_LEFT: sense = -1;
        case SDLK_RIGHT:
            if (canPieceMove(*thePiece, sense))
                (*thePiece).coors[0] += sense;
            break;
        case SDLK_DOWN:
            while (!hasPieceFallen(*thePiece))
                (*thePiece).coors[1]++;
            break;
        case SDLK_SPACE:
            if(canPieceRotate(*thePiece)) 
                rotatePiece(thePiece);
    }
}

void handleInput(piece* aPiece) {
    long start = clock();
    while((clock() - start)/CLOCKS_PER_SEC < FRAMETIME) {
        int key = gfx_pollkey();
        if (key != -1) {
            handleAction(key, aPiece);
            render(*aPiece);
        }
    }
}

void run() {
    while (!isGameLost()) {
        piece thePiece = initPiece();
        do {
            thePiece.coors[1]++;
            render(thePiece);
            handleInput(&thePiece);
        } while (!hasPieceFallen(thePiece));

        int row;
        lockPiece(thePiece);
        while((row = getFullRow()) != -1) collapseRow(row);
    }
}

void checkConstants() {
    int totalHeight = BOARD_HEIGHT + 2*(MARGIN+BORDER);
    int totalWidth = BOARD_WIDTH + 2*(MARGIN+BORDER);
    assert(totalHeight <= gfx_screenHeight());
    assert(totalWidth <= gfx_screenWidth());
}

int main() {
    if (gfx_init()) abort();
    checkConstants();
    initVariables();
    srand((unsigned) time(NULL));
    run();
    return 0;
}
