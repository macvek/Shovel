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
            else if (k.type >= STANDARD) {
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
