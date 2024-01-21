
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

int x = 39, y = 11;
int mx = 1, my = 0;

typedef pair<int,int> Point;

deque<Point> points;

void gameFrame() {
    
    x+= mx;
    y+= my;

    if (x < 1) {
        x = 78;
    }
    else if (x > 78) {
        x = 1;
    }
    else if (y < 1) {
        y = 22;
    }
    else if (y > 22) {
        y = 1;
    }
    
    points.push_back({x,y});

    if (points.size() > 5) {
        points.pop_front();
    }
}

void render() {
    backBuffer.copyFrom(frontBuffer);
    frontBuffer.copyFrom(blankScene);
    
    f.drawFrame(frontBuffer,0,0,79,23, Frame::SingleBorder);
    for (auto ptr = points.cbegin(); ptr < points.cend(); ++ptr) {
        frontBuffer.writeText("X", ptr->first, ptr->second);
    }
    

    frontBuffer.diff(backBuffer, diffs, 3);
    frontBuffer.unitsToTerminal(t, diffs, 1, 1, true);
    
    diffs.clear();
    t.flush();
}

struct Snake : public TimerOnTick {
    void onTick() {
        gameFrame();
        render();
    }
};

int main(int argc, char** argv) {
    console.enableRaw();
    t.showCursor(false);
    t.clear();

    Snake snake;

    Timer timer(100, &snake);

    render();
    timer.start();
    for (;;) {
        input.waitFor();
        Key k = input.getKey();
        if (k.type == ESCAPE) {
            timer.stop();
            break;
        }

        if (k.type == ARROW_RIGHT) {
            mx = 1; my = 0;
        }
        if (k.type == ARROW_LEFT) {
            mx = -1; my = 0;
        }
        if (k.type == ARROW_UP) {
            my = -1; mx = 0;
        }
        if (k.type == ARROW_DOWN) {
            my = 1; mx = 0;
        }
    }

    t.showCursor(true);
    return 0;
}
 