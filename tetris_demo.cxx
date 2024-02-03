#include "console.h"
#include "input.h"
#include "terminal.h"
#include "renderbuffer.h"
#include "frame.h"
#include "timer.h"
#include "log.h"
#include <cstdlib>
#include <vector>
#include <array>

using namespace std;

Console console;
Input input;
Terminal t(std::cout);
SpecialCharsMap base = SpecialCharsMapFactory::create();
RenderBuffer blankScene(80,24,'.',true, base);
RenderBuffer backBuffer = blankScene;
RenderBuffer frontBuffer = blankScene;
RenderBuffer blocksBuffer(20,22,' ', true, base);

array<char, 20*22> blocks;


vector<RenderUnit> diffs;
Frame f;

int cursorX, cursorY;

int frameNo = 0;
int levelWidth = 20;
int levelHeight = 22;

int levelFrames;
int nextFrameDown;
bool moveEveryFrame = false;

bool gameOver = false;
bool pause = false;
int tileSize = 4;

string partA = 
"@@@@"
"    "
"    "
"    "
;

string partB = 
" @  "
"@X@ "
"    "
"    "
;

string partC = 
"@   "
"@X@ "
"    "
"    "
;

string partD = 
"@@  "
"@@  "
"    "
"    "
;

string partE = 
"  @ "
"@X@ "
"    "
"    "
;

string partF = 
"@@  "
" @@ "
"    "
"    "
;

array<string*, 6> parts = {
    &partA, &partB, &partC, &partD, &partE, &partF
};

array<Terminal::COLOR, 6> colors = {
    Terminal::COLOR::BRIGHT_BLUE,
    Terminal::COLOR::BRIGHT_RED,
    Terminal::COLOR::BRIGHT_YELLOW,
    Terminal::COLOR::BRIGHT_CYAN,
    Terminal::COLOR::BRIGHT_GREEN,
    Terminal::COLOR::BRIGHT_MAGENTA,
};

TermColor shadowTileColor = Terminal::MakeColor(Terminal::COLOR::BRIGHT_WHITE, Terminal::COLOR::DEFAULT);

TermColor currentTileColor;
string currentTile = partA;
int currentTileIdx;

TermColor nextTileColor;
string nextTile;
int nextTileIdx;

int gameLines;
int gameMoves;
int gameSpeed;

void renderGameOver() {
    f.drawFrame(frontBuffer,30,8,50,12, Frame::DoubleBorder);
    frontBuffer.writeText("Game Over", 36, 10);
    frontBuffer.writeText("[ENTER] - try again", 31, 13);
    
    TermColor red = Terminal::MakeColor(Terminal::RED, Terminal::DEFAULT);

    frontBuffer.writeColorLine(30,8, 21, red);
    frontBuffer.writeColorLine(30,9, 21, red);
    frontBuffer.writeColorLine(30,10, 21, red);
    frontBuffer.writeColorLine(30,11, 21, red);
    frontBuffer.writeColorLine(30,12, 21, red);
}

int tileXY(int x, int y) {
    return y* tileSize + x;
}

void trimTile(string& toTrim) {
    int lastEmptyLine = -1;
    int lastEmptyColumn = tileSize;
    for (int y=0;y<tileSize;++y) {
        bool emptyLine = true;
        int emptyColumnIdx = -1;
        for (int x=0;x<tileSize;++x) {
            if ( toTrim[tileXY(x,y)] != ' ') {
                emptyLine = false;
            }
            else if (emptyColumnIdx == x - 1) {
                emptyColumnIdx = x;
            }
        }

        lastEmptyColumn = min(lastEmptyColumn, emptyColumnIdx);

        if (emptyLine && lastEmptyLine == y-1) {
            lastEmptyLine = y;
        }
    }
    
    int tX = lastEmptyColumn + 1;
    int tY = lastEmptyLine + 1;

    for (int y=0;y<tileSize;++y) for (int x=0;x<tileSize;++x) {
        int fromX = x + tX;
        int fromY = y + tY;
        
        if (fromX >= tileSize || fromY >= tileSize) {
            toTrim[ tileXY(x,y) ] = ' ';
        }
        else {
            toTrim[ tileXY(x,y) ] = toTrim[ tileXY(fromX, fromY)];
        }
    }
}

int xy(int x , int y) {
    return y* levelWidth + x;
}

bool cursorCollides(int checkX, int checkY) {
    if (checkX < 0) {
        return true;
    }

    for (int y=0;y<tileSize;++y) {
        bool anyTile = false;
        for (int x=0;x<tileSize;++x) {
            if (currentTile[tileXY(x,y)] != ' ') {
                anyTile = true;
                if (x + checkX >= levelWidth) {
                   return true;
                }
            }
        }

        if (!anyTile) {
            return false;
        }

        if (checkY + y >= levelHeight ) {
            return true;
        }
        
        for (int x=0;x<tileSize;++x) {
            if (currentTile[tileXY(x,y)] != ' ' && blocks[xy(checkX + x, checkY + y)]) {
                return true;
            }
        }
    }

    return false;
}

void rotate() {
    string rotated = currentTile;
    string original = currentTile;
    int ptr = 0;
    
    int pivotX = -1;
    int pivotY = -1;

    // rotated loop
    for (int x=0;x<tileSize;++x) for (int y=tileSize-1; y>=0 ;--y) {
        char tileValue = currentTile[tileXY(x,y)];
        if (tileValue == 'X') {
            pivotX = x;
            pivotY = y;
        }
        rotated[ptr] = tileValue;
        ++ptr;
    }

    trimTile(rotated);

    int rotatedPivotX = -1;
    int rotatedPivotY = -1;

    for (int y=0;y<tileSize;++y) for (int x=0;x<tileSize;++x) {
        char tileValue = rotated[tileXY(x,y)];
        if (tileValue == 'X') {
            rotatedPivotX = x;
            rotatedPivotY = y;
            break;
        }
    }

    int tX = 0;
    int tY = 0;
    if (rotatedPivotX != -1 && rotatedPivotY != -1 && pivotX != -1 && pivotY != -1) {
        tX = pivotX - rotatedPivotX;
        tY = pivotY - rotatedPivotY;
    }

    currentTile = rotated;
    if (cursorCollides(cursorX+tX, cursorY+tY)) {
        currentTile = original;
    }
    else {
        cursorX += tX;
        cursorY += tY;
    }
}

void moveCursor(int xOffset) {
    if (!cursorCollides(cursorX+xOffset, cursorY)) {
        cursorX = cursorX+xOffset;
        ++gameMoves;
    }
}

void randomizeNextTile() {
    nextTileIdx = rand() % parts.size();
    nextTile = *parts[nextTileIdx];
    nextTileColor = Terminal::MakeColor(colors[nextTileIdx], Terminal::DEFAULT);
}

void pickTiles() {
    currentTileIdx = nextTileIdx;
    currentTile = nextTile;
    currentTileColor = nextTileColor;
    
    randomizeNextTile();
    
}

void resetCursor() {
    cursorX = 8;
    cursorY = 0;

    pickTiles();
    gameOver = cursorCollides(cursorX, cursorY);
    moveEveryFrame = false;
}

void refreshBlocksBuffer() {
    string off = " ";
    string on = "@";

    for (int y=0;y<levelHeight;++y) for (int x=0;x<levelWidth;++x) {
        char blockValue = blocks[xy(x,y)];
        if (!blockValue) {
            blocksBuffer.writeText(off, x, y);
        }
        else {
            blocksBuffer.writeText(on, x, y);
            auto color = Terminal::MakeColor(colors[blockValue - '0' ], Terminal::COLOR::DEFAULT);
            blocksBuffer.writeColorLine(x,y,1, color);
        }
    }
}

void clearBlocks() {
    for (int i=0;i<blocks.size(); ++i) blocks[i] = 0;    
    refreshBlocksBuffer();
}

void calcLevelFrames() {
    if (gameLines > 20) levelFrames = 3;
    else if (gameLines > 18) levelFrames = 4;
    else if (gameLines > 15) levelFrames = 5;
    else if (gameLines > 12) levelFrames = 6;
    else if (gameLines > 9) levelFrames = 7;
    else if (gameLines > 6) levelFrames = 8;
    else if (gameLines > 3) levelFrames = 9;
    else levelFrames = 10;

    gameSpeed = 11 - levelFrames;

}

void resetGame() {
    gameLines = 0;
    gameMoves = 0;
    gameSpeed = 1;

    calcLevelFrames();
    randomizeNextTile();
    clearBlocks();
    resetCursor();
    frameNo = 0;
    nextFrameDown = frameNo + levelFrames;
}

void clearBlockLine(int y) {
    for (int x = 0; x < levelWidth; ++x ) {
        blocks[xy(x,y)] = 0;
    }
}

void copyFromAboveLine(int y) {
    for (int x = 0; x < levelWidth; ++x ) {
        blocks[xy(x,y)] = blocks[xy(x,y-1)];
    }
}

void fillUpBlockLine(int y) {
    if (y == 0) {
        clearBlockLine(y);
    }
    else {
        copyFromAboveLine(y);
    }
}

void removeBlockLine(int removeY) {
    for (int y = removeY; y>=0; --y) {
        fillUpBlockLine(y);
    }
}

void clearFullLines() {
    for (int y=0;y<levelHeight;++y) {
        bool full = true;
        for (int x=0;x<levelWidth;++x) {
            if (!blocks[xy(x,y)]) {
                full = false;
                break;
            }
        }

        if (full) {
            removeBlockLine(y);
            ++gameLines;
            calcLevelFrames();
        }
    } 
}

void placeTile(int placeY) {
    int placeX = cursorX;
    
    for (int y=0;y<tileSize;++y) for (int x=0;x<tileSize;++x) {
        if (currentTile[tileXY(x,y)] != ' ') {
            blocks[xy(placeX+x, placeY+y)] = '0'+currentTileIdx;
        }
    }
    
    clearFullLines();
    refreshBlocksBuffer();
}


void gameFrame() {
    if (pause) {
        return;
    }

    ++frameNo;
    if (gameOver) {
        return;
    }
    
    if (moveEveryFrame || frameNo == nextFrameDown) {
        nextFrameDown = frameNo + levelFrames;

        if (cursorCollides(cursorX, 1+cursorY)) {
            placeTile(cursorY);
            resetCursor();
        }
        else {
            ++cursorY;
        }
    }
}

void renderTile(int offX, int offY, std::string& tileSource, TermColor tileColor) {
    for (int y=0;y<tileSize;++y) for (int x=0;x<tileSize;++x) {
        if (tileSource[tileXY(x,y)] != ' ') {
            frontBuffer.writeText("@", offX+x, offY+y);
            frontBuffer.writeColorLine(offX+x, offY+y, 1, tileColor);
        }
    }
}

const int blocksXOff = 30;
const int blocksYOff = 1;

void renderShadow() {
    int shadowY = cursorY;
    for (; !cursorCollides(cursorX, 1 + shadowY); ++shadowY );
    if (shadowY > cursorY + 3) {
        renderTile(blocksXOff + cursorX, blocksYOff + shadowY, currentTile, shadowTileColor);
    }
}

bool renderInProgress = false;
void render() {
    if (renderInProgress) return;
    renderInProgress = true;

    backBuffer.copyFrom(frontBuffer);
    frontBuffer.copyFrom(blankScene);
    
    f.drawFrame(frontBuffer,0,0,79,23, Frame::SingleBorder);
    f.drawFrame(frontBuffer,29,0,50,23, Frame::SingleHorisontalExtend);
    f.drawFrame(frontBuffer,12,3,17,6, Frame::SingleBorder);
    renderTile(13, 4, nextTile, nextTileColor);

    stringstream linesLabel; linesLabel << " Lines: " << gameLines << " ";
    stringstream movesLabel; movesLabel << " Moves: " << gameMoves << " ";
    stringstream speedLabel; speedLabel << " Speed: " << gameSpeed << " ";

    frontBuffer.writeText(linesLabel.str(), 11, 7);
    frontBuffer.writeText(movesLabel.str(), 11, 8);
    frontBuffer.writeText(speedLabel.str(), 11, 9);

    frontBuffer.writeText(" ESC - exit ", 66,0);
    
    frontBuffer.writeView(blocksBuffer.view(), blocksXOff,blocksYOff);
    renderTile(blocksXOff + cursorX, blocksYOff + cursorY, currentTile, currentTileColor);
   
    if (gameOver) {
        renderGameOver();
    }
    else {
        renderShadow();
    }

    frontBuffer.diff(backBuffer, diffs, 1);
    frontBuffer.unitsToTerminal(t, diffs, 1, 1, true);
    diffs.clear();

    t.flush();
    renderInProgress = false;
}

struct Tetris : public TimerOnTick {
    void onTick() {
        gameFrame();
        render();
    }
};

int main(int argc, char** argv) {
    srand(0);
    console.enableRaw();
    t.showCursor(false);
    t.clear();
    blankScene.toTerminal(t, 1,1, true);

    Tetris tetris;
    Timer timer(30, &tetris);

    resetGame();
    render();
    timer.start();
    for (;;) {
        input.waitFor();
        Key k = input.getKey();
        if (k.type == ESCAPE) {
            timer.stop();
            break;
        }

        if (!gameOver) {
            if (k.value == 'p') {
                pause = !pause;
            }
            if (k.value == 'r') {
                render();
            }

            if (!pause) {
                if (k.type == ARROW_LEFT) {
                    moveCursor(-1);
                    render();
                }
                else if (k.type == ARROW_RIGHT) {
                    moveCursor(1);
                    render();
                }
                else if (k.type == ARROW_UP) {
                    ++gameMoves;
                    rotate();
                    render();
                }
                else if (k.type == ARROW_DOWN) {
                    moveEveryFrame = true;
                }
            }
        }

        if (gameOver && k.type == ENTER) {
            resetGame();
        }
    }

    t.moveCursor(0,23);
    t.stream() << endl;
    t.showCursor(true);
    return 0;
}
 