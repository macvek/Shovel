/*
MIT License

Copyright (c) 2024 Maciej Aleksandrowicz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdio.h>

#include "console.h"
#include "input.h"
#include "terminal.h"
#include "renderbuffer.h"
#include "log.h"

using namespace std;

int main(int argc, char** argv) {
    Console console;
    Input input;
    Terminal t(std::cout);
    RenderBuffer b(23,7, 0, true);
    b.writeText(
        "                       "
        " --------------------- "
        " ------         ------ "
        " -----     +     ----- "
        " ------         ------ "
        " --------------------- "
        "                       ",
    0,0);

    b.writeColorLine(1,1,21, Terminal::MakeColor(Terminal::MAGENTA, Terminal::RED));
    b.writeColorLine(1,5,21, Terminal::MakeColor(Terminal::MAGENTA, Terminal::RED));

    RenderBuffer scene(80,24,'.',true);

    Terminal::COLOR colors[] = {
        Terminal::BLACK,
        Terminal::RED,
        Terminal::GREEN,
        Terminal::YELLOW,
        Terminal::BLUE,
        Terminal::MAGENTA,
        Terminal::CYAN,
        Terminal::WHITE,
    };
    
    int renderColor = 7;

    int diffThreshold = 0;
    bool fullWrite = true;
    bool coloring = true;
    try {
        console.enableRaw();
        t.clear();
        int x = 0;
        int y = 0;
        t.placeCursor(1,1);
        t.stream() << "TOGGLES: TAB(full write) / ENTER(coloring) / 0-9(diff threshold)";
        scene.toTerminal(t,1,2);
        scene.writeView(b.view(), x,y);
        t.flush();
        vector<RenderUnit> diffs;
        for(;;) {
            RenderBuffer backBuffer(scene);
            scene.writeView(b.view(), x,y);
            if (coloring) {
                t.foreColor(colors[renderColor % 8]);
                t.backColor(colors[ (1+renderColor) % 8]);
            }
            else {
                t.foreDefault();
                t.backDefault();
            }
            ++renderColor;
            
            if (fullWrite) {
                scene.toTerminal(t,1,2,!coloring);
            }
            else {
                scene.diff(backBuffer, diffs, diffThreshold);
                scene.unitsToTerminal(t, diffs, 1, 2, !coloring);
                diffs.clear();
            }
            t.flush();

            input.waitFor();
            Key k = input.getKey();
            if (k.type == ESCAPE) {
                return 0;
            }
            else if (k.type == TAB) {
                fullWrite = !fullWrite;
            }
            else if (k.type == ENTER) {
                coloring = !coloring;
            }
            else if (k.type == ARROW_RIGHT) {
                ++x;
            }
            else if (k.type == ARROW_LEFT) {
                --x;
            }
            else if (k.type == ARROW_DOWN) {
                ++y;
            }
            else if (k.type == ARROW_UP) {
                --y;
            }
            else if (k.value >= '0' && k.value <= '9') {
                diffThreshold = k.value - '0';
            }
        }
    }
    catch (LogPanic& panic) {

    }
}
