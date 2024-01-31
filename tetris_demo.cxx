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
RenderBuffer blankScene(80,24,' ',true, base);
RenderBuffer backBuffer = blankScene;
RenderBuffer frontBuffer = blankScene;
RenderBuffer blocksBuffer(20,22,' ', true, base);
RenderBuffer blocksBackBuffer = blocksBuffer;

array<char, 20*22> blocks;


vector<RenderUnit> diffs;
Frame f;

int cursorX, cursorY;

int frameNo = 0;
int levelFrames = 3;
int levelWidth = 20;
int levelHeight = 22;
int nextFrameDown;
bool moveEveryFrame = false;

bool gameOver;
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

array<string*, 5> parts = {
    &partA, &partB, &partC, &partD, &partE
};


string currentTile;

void renderGameOver() {
    f.drawFrame(frontBuffer,30,8,50,12, Frame::DoubleBorder);
    frontBuffer.writeText("Game Over", 36, 10);
    frontBuffer.writeText("[ENTER] - try again", 31, 13);
    
    frontBuffer.writeColorLine(30,8, 21, Terminal::MakeColor(Terminal::RED, Terminal::DEFAULT));
    frontBuffer.writeColorLine(30,9, 21, Terminal::MakeColor(Terminal::RED, Terminal::DEFAULT));
    frontBuffer.writeColorLine(30,10, 21, Terminal::MakeColor(Terminal::RED, Terminal::DEFAULT));
    frontBuffer.writeColorLine(30,11, 21, Terminal::MakeColor(Terminal::RED, Terminal::DEFAULT));
    frontBuffer.writeColorLine(30,12, 21, Terminal::MakeColor(Terminal::RED, Terminal::DEFAULT));
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

int range(int what, int min, int max) {
    return what < min ? min : what > max ? max : what;
}

bool cursorCollides(int checkX, int checkY) {

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
    cursorX += tX;
    cursorY += tY;

    if (cursorCollides(cursorX, cursorY)) {
        currentTile = original;
        cursorX -= tX;
        cursorY -= tY;
    }
}

void moveCursor(int xOffset) {
    int newValue = range(cursorX+xOffset, 0, levelWidth-1);
    if (!cursorCollides(newValue, cursorY)) {
        cursorX = newValue;
    }
}

void pickTiles() {
    int idx = rand() % parts.size();
    currentTile = *parts[idx];
}

void resetCursor() {
    cursorX = 3;
    cursorY = 0;
    gameOver = cursorCollides(cursorX, cursorY);
    moveEveryFrame = false;
    
    pickTiles();
}

void refreshBlocksBackBuffer() {
    string off = " ";
    
    for (int y=0;y<levelHeight;++y) for (int x=0;x<levelWidth;++x) {
        auto block = blocks[xy(x,y)];
        string val = off;
        if (block) {
            val[0] = block;
        }
        blocksBackBuffer.writeText(val, x, y);
    }
}

void clearBlocks() {
    for (int i=0;i<blocks.size(); ++i) blocks[i] = 0;    
    refreshBlocksBackBuffer();
}

void resetGame() {
    clearBlocks();
    resetCursor();
    frameNo = 0;
    nextFrameDown = frameNo + levelFrames;
}

int currentColor = 0;
void placeTile(int placeY) {
    int placeX = cursorX;
    
    for (int y=0;y<tileSize;++y) for (int x=0;x<tileSize;++x) {
        if (currentTile[tileXY(x,y)] != ' ') {
            blocks[xy(placeX+x, placeY+y)] = '0' + (currentColor%8);
        }
    }
    ++currentColor;
    
    refreshBlocksBackBuffer();
}


void gameFrame() {
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

void renderTile() {
    for (int y=0;y<tileSize;++y) for (int x=0;x<tileSize;++x) {
        if (currentTile[tileXY(x,y)] != ' ') {
            blocksBuffer.writeText("@", cursorX+x, cursorY+y);
        }
    }
}

void render() {
    backBuffer.copyFrom(frontBuffer);
    frontBuffer.copyFrom(blankScene);
    
    f.drawFrame(frontBuffer,0,0,79,23, Frame::SingleBorder);
    
    stringstream scoreLabel;
    scoreLabel << " Frame: " << frameNo << " ";

    frontBuffer.writeText(scoreLabel.str(), 2,0);
    frontBuffer.writeText(" ESC - exit ", 66,0);

    blocksBuffer.copyFrom(blocksBackBuffer);
    renderTile();

    frontBuffer.writeView(blocksBuffer.view(), 30,1);

    if (gameOver) {
        renderGameOver();
    }


    frontBuffer.diff(backBuffer, diffs, 3);
    frontBuffer.unitsToTerminal(t, diffs, 1, 1, true);
    
    diffs.clear();
    t.flush();
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

    Tetris tetris;
    Timer timer(50, &tetris);

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
            if (k.type == ARROW_LEFT) {
                moveCursor(-1);
                render();
            }
            else if (k.type == ARROW_RIGHT) {
                moveCursor(1);
                render();
            }
            else if (k.type == ARROW_UP) {
                rotate();
                render();
            }
            else if (k.type == ARROW_DOWN) {
                moveEveryFrame = true;
            }
        }

        if (gameOver && k.type == ENTER) {
            resetGame();
        }
    }

    t.showCursor(true);
    return 0;
}
 