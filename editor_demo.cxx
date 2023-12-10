#include <stdio.h>
#include <unistd.h>

#include "console.h"
#include "inputdecoder.h"
#include "log.h"
#include "output.h"
#include "editor.h"
#include "terminal.h"

using namespace std;

int main(int argc, char** argv) {
    Console console;
    InputDecoder decoder;
    Terminal t(cout);
    Editor e;
    try {
        console.enableRaw();
        t.clear();
        cout.flush();
        bool cursorIsVisible = true;
        for(;;) {
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
                else if (k.type == F2) {
                    cursorIsVisible = !cursorIsVisible;
                    t.showCursor(cursorIsVisible);
                    cout.flush();
                }
                else {
                    e.consume(k);
                    t.clear();
                    cout << e.getText();
                    t.placeCursor(1+e.getCurrentColumn(), 1+e.getCurrentLine());
                    cout.flush();
                }
            }
        }
    }
    catch (LogPanic& panic) {

    }
}
