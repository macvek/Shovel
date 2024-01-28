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

array<bool, 20*22> blocks;


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

string dot = 
"@   "
"    "
"    "
"    "
;

string partA = 
"@@@@"
"    "
"    "
"    "
;

string currentTile = partA;

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

int xy(int x , int y) {
    return y* levelWidth + x;
}

int range(int what, int min, int max) {
    return what < min ? min : what > max ? max : what;
}

bool cursorCollides(int checkX, int checkY) {
    if (checkY >= levelHeight ) {
        return true;
    }

    int counter = 0;
    for (int y=0;y<tileSize;++y)
    for (int x=0;x<tileSize;++x) {
        if (currentTile[counter] != ' ' && blocks[xy(checkX + x, checkY + y)]) {
            return true;
        }
        ++counter;
    }

    return false;
}

void moveCursor(int xOffset) {
    int newValue = range(cursorX+xOffset, 0, levelWidth-1);
    if (!cursorCollides(newValue, cursorY)) {
        cursorX = newValue;
    }
}

void resetCursor() {
    cursorX = 3;
    cursorY = 0;
    gameOver = cursorCollides(cursorX, cursorY);
    moveEveryFrame = false;
}

void refreshBlocksBackBuffer() {
    string on = "@";
    string off = " ";
    
    for (int y=0;y<levelHeight;++y)
    for (int x=0;x<levelWidth;++x) {
        blocksBackBuffer.writeText(blocks[xy(x,y)] ? on : off, x, y);
    }
}

void clearBlocks() {
    for (int i=0;i<blocks.size(); ++i) blocks[i] = false;    
    refreshBlocksBackBuffer();
}

void resetGame() {
    clearBlocks();
    resetCursor();
    frameNo = 0;
    nextFrameDown = frameNo + levelFrames;
}

void placeTile(int placeY) {
    int placeX = cursorX;
    
    int counter = 0;
    for (int y=0;y<tileSize;++y)
    for (int x=0;x<tileSize;++x) {
        if (currentTile[counter] != ' ') {
            blocks[xy(placeX+x, placeY+y)] = true;
        }
        ++counter;
    }

    
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
    int counter = 0;
    for (int y=0;y<tileSize;++y)
    for (int x=0;x<tileSize;++x) {
        if (currentTile[counter] != ' ') {
            blocksBuffer.writeText("@", cursorX+x, cursorY+y);
        }
        ++counter;
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
 