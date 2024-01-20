#include <stdio.h>

#include "console.h"
#include "input.h"
#include "terminal.h"
#include "renderbuffer.h"
#include "frame.h"
#include "timer.h"
#include "log.h"

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

int offset = 0;


void render() {
    backBuffer.copyFrom(frontBuffer);
    frontBuffer.copyFrom(blankScene);
    f.drawFrame(frontBuffer,0,0,79,23, Frame::SingleBorder);
    frontBuffer.writeText("HelloWorld", offset,offset);
    ++offset;

    frontBuffer.diff(backBuffer, diffs, 3);
    frontBuffer.unitsToTerminal(t, diffs, 1, 1, true);
    
    diffs.clear();
    t.flush();
}

struct Snake : public TimerOnTick {
    void onTick() {
        render();
    }
};

int main(int argc, char** argv) {
    console.enableRaw();
    t.showCursor(false);
    t.clear();

    Snake snake;

    Timer timer(500, &snake);

    render();
    timer.start();
    for (;;) {
        input.waitFor();
        Key k = input.getKey();
        if (k.type == ESCAPE) {
            timer.stop();
            break;
        }
    }

    return 0;
}
