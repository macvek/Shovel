#include "console.h"
#include "input.h"
#include "terminal.h"
#include "renderbuffer.h"
#include "frame.h"
#include "timer.h"
#include "log.h"
#include <cstdlib>


using namespace std;

Console console;
Input input;
Terminal t(std::cout);
SpecialCharsMap base = SpecialCharsMapFactory::create();
RenderBuffer blankScene(80,24,' ',true, base);
RenderBuffer backBuffer = blankScene;
RenderBuffer frontBuffer = blankScene;
vector<RenderUnit> diffs;
Frame f;
int frameNo = 0;

bool gameOver;

void renderGameOver() {
    f.drawFrame(frontBuffer,30,8,50,12, Frame::DoubleBorder);
    frontBuffer.writeText("Game Over", 36, 10);
    frontBuffer.writeText("[ENTER] - try again", 31, 14);
    
    frontBuffer.writeColorLine(30,8, 21, Terminal::MakeColor(Terminal::RED, Terminal::DEFAULT));
    frontBuffer.writeColorLine(30,9, 21, Terminal::MakeColor(Terminal::RED, Terminal::DEFAULT));
    frontBuffer.writeColorLine(30,10, 21, Terminal::MakeColor(Terminal::RED, Terminal::DEFAULT));
    frontBuffer.writeColorLine(30,11, 21, Terminal::MakeColor(Terminal::RED, Terminal::DEFAULT));
    frontBuffer.writeColorLine(30,12, 21, Terminal::MakeColor(Terminal::RED, Terminal::DEFAULT));

}

void resetGame() {
    gameOver = false;
    frameNo = 0;
}

void gameFrame() {
    ++frameNo;
    if (gameOver) {
        return;
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

        if (gameOver && k.type == ENTER) {
            resetGame();
        }
    }

    t.showCursor(true);
    return 0;
}
 