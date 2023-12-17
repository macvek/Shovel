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
    RenderBuffer b(23,7);
    b.writeText(
        "                       "
        " --------------------- "
        " ------         ------ "
        " -----     +     ----- "
        " ------         ------ "
        " --------------------- "
        "                       ",
    0,0);

    RenderBuffer scene(80,24,'.');
    try {
        console.enableRaw();
        t.clear();
        int x = 0;
        int y = 0;
        for(;;) {
            scene.writeView(b.view(), x,y);
            scene.toTerminal(t,1,1);
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
            }
        }
    }
    catch (LogPanic& panic) {

    }
}
