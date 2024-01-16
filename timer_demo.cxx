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
