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
int score;

int x, y, mx, my;

#define NEXT_GOAL_COUNTER 30
int nextGoal;

typedef pair<int,int> Point;

deque<Point> points;
Point lastRemoved;
vector<Point> goals;

void resetGame() {
    x = 39;
    y = 11;
    lastRemoved = {x-1, y};
    mx = 1;
    my = 0;
    score = 0;
    nextGoal = 1;
    points.clear();
    goals.clear();
}

void gameOver() {
    resetGame();
}

bool checkCollision() {
    for (auto i = points.cbegin(); i < points.cend(); ++i) {
        if (x == i->first && y == i->second ) {
            return true;
        }
    }

    return false;
}

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
    
    if (checkCollision()) {
        gameOver();
        return;
    }

    points.push_back({x,y});

    bool hasGoal = false;
    for (auto i = goals.cbegin(); i < goals.cend(); ++i) {
        if (x == i->first && y == i->second ) {
            hasGoal = true;
            ++score;
            goals.erase(i);
            break;
        }
    }

    if (points.size() > 5 && !hasGoal) {
        lastRemoved = points.front();
        points.pop_front();
    }

    if (--nextGoal == 0) {
        nextGoal = NEXT_GOAL_COUNTER;
        int x = 1 + (rand() % 78);
        int y = 1 + (rand() % 22);
        goals.push_back({x,y});
    }
}

string HeadRight = ">";
string HeadLeft = "<";
string HeadUp= "^";
string HeadDown = "v";

string UnknownSign = "?";
string Horisontal = "\xC4";
string Vertical = "\xB3";
string TurnA = "\xBF";
string TurnB = "\xD9";
string TurnC = "\xC0";
string TurnD = "\xDA";

pair<Point&, Point&> sortPoints(Point& a, Point& b) {
    if (a.first <= b.first) {
        return {a,b};
    }
    else {
        return {b,a};
    }
}

string& CharForPoints(Point a, Point b) {
    auto sorted = sortPoints(a,b);
    Point& left = sorted.first;
    Point& right = sorted.second;

    if (left.first == right.first) {
        return Vertical;
    }
    else if (left.second == right.second) {
        return Horisontal;
    }

    if (left.second == 0) {
        if (right.second > 0) {
            return TurnA;
        }
        else {
            return TurnB;
        }
    
/*
xA
 x
 
 x
xB
*/
    } else {
        if (left.second < 0) {
            return TurnC;
        }
        else {
            return TurnD;
        }
/*
x
Cx

Dx
x
*/
    }

    return UnknownSign;
}

void render() {
    backBuffer.copyFrom(frontBuffer);
    frontBuffer.copyFrom(blankScene);
    
    f.drawFrame(frontBuffer,0,0,79,23, Frame::SingleBorder);
    
    stringstream scoreLabel;
    scoreLabel << " Score: " << score << " ";

    frontBuffer.writeText(scoreLabel.str(), 2,0);

    for (auto ptr = goals.cbegin(); ptr < goals.cend(); ++ptr) {
        frontBuffer.writeText("*", ptr->first, ptr->second);
    }
    
    if (points.size() > 1) {
        auto tail = *points.cbegin();
        auto next = *(points.cbegin()+1);
        
        frontBuffer.writeText(CharForPoints(
            {lastRemoved.first - tail.first, lastRemoved.second - tail.second},
            {next.first - tail.first, next.second - tail.second}),
             tail.first, tail.second);
    }

    if (points.size() > 2) {
        for (auto ptr = points.cbegin() + 1; ptr < points.cend() - 1; ++ptr) {

            const Point& here = *ptr;
            const Point& prev = *(ptr - 1);
            const Point& next = *(ptr + 1);


            frontBuffer.writeText(CharForPoints(
                { next.first - here.first, next.second - here.second },
                { prev.first - here.first, prev.second - here.second }),
                here.first, here.second);
        }
    }

    if (points.size() > 0) {
        auto head = points.cend()-1;
        string* headChar;
        if (mx == 1) {
            headChar = &HeadRight;
        }
        else if (mx == -1) {
            headChar = &HeadLeft;
        }
        else if (my == -1) {
            headChar = &HeadUp;
        }
        else {
            headChar = &HeadDown;
        }
        frontBuffer.writeText(*headChar, head->first, head->second);
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
    srand(0);
    console.enableRaw();
    t.showCursor(false);
    t.clear();

    Snake snake;

    Timer timer(50, &snake);

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

        if (k.type == ENTER) {
            resetGame();
        }
        if (k.type == ARROW_RIGHT && mx != -1) {
            mx = 1; my = 0;
        }
        if (k.type == ARROW_LEFT && mx != 1) {
            mx = -1; my = 0;
        }
        if (k.type == ARROW_UP && my != 1) {
            my = -1; mx = 0;
        }
        if (k.type == ARROW_DOWN && my != -1) {
            my = 1; mx = 0;
        }
    }

    t.showCursor(true);
    return 0;
}
 