#include <stdio.h>
#include "input.h"
#include "console.h"
#include "log.h"
#include "output.h"
#include "editor.h"
#include "terminal.h"

using namespace std;

int main(int argc, char** argv) {
    Console console;
    Input input;
    Terminal t(cout);
    Editor e;
    try {
        console.enableRaw();
        t.clear();
        cout.flush();
        bool cursorIsVisible = true;
        for(;;) {
            input.waitFor();
            Key k = input.getKey();
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
    catch (LogPanic& panic) {

    }
}
