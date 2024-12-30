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

#include "console.h"
#include "timer.h"
#include "log.h"
#include "input.h"
#include "output.h"
#include <stdio.h>
using namespace std;

struct MyOnTick : public TimerOnTick {
    void onTick() {
        std::cout << "On My Tick" << ENDLINE;
    }
};

void showUsage() {
    cout << "r -> resume timer; s -> stop timer; ESC -> exit" << endl;
}

int main(int argc, char** argv) {
    Console console;
    MyOnTick myOnTick;
    Input input;
    showUsage();
    try {
        Timer timer(200, &myOnTick);
        console.enableRaw();

        for (;;) {
            input.waitFor();
            Key what = input.getKey();
            if (what.type < STANDARD) {
                continue;
            }

            if (what.type == ESCAPE) {
                cout << "I'm done" << ENDLINE;
                break;
            } 
            else if (what.value == 's') {
                cout << "Stopping timer" << ENDLINE;
                timer.stop();
            } 
            else if (what.value == 'r') {
                cout << "Starting timer" << ENDLINE;
                timer.start();
            } 
            else {
                showUsage();
            }
        }
    }
    catch (LogPanic& panic) {

    }
}
