#include <stdio.h>
#include <unistd.h>

#include "console.h"
#include "inputdecoder.h"
#include "terminal.h"
#include "renderbuffer.h"
#include "log.h"

using namespace std;

int main(int argc, char** argv) {
    Console console;
    InputDecoder decoder;
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

    b.writeColorLine(0,1,23, Terminal::MakeColor(Terminal::MAGENTA, Terminal::RED));
    b.writeColorLine(0,5,23, Terminal::MakeColor(Terminal::MAGENTA, Terminal::RED));

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
        scene.toTerminal(t,1,1);
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
                scene.toTerminal(t,1,1,!coloring);
            }
            else {
                scene.diff(backBuffer, diffs, diffThreshold);
                scene.unitsToTerminal(t, diffs, 1, 1);
                diffs.clear();
            }
            t.flush();

            AChar buffer[16];
            int ret = read(STDIN_FILENO, buffer, 8);
            if (ret <= 0) {
                return 0;
            }

            decoder.feed(buffer, ret);
            while(decoder.canLoad()) {
                Key k = decoder.load();
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
    }
    catch (LogPanic& panic) {

    }
}
