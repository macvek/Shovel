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

int main(int argc, char** argv) {
    Console console;
    MyOnTick myOnTick;
    Input input;
    try {
        Timer timer(200, &myOnTick);
        console.enableRaw();

        for (;;) {
            input.waitFor();
            Key what = input.getKey();

            if (what.value == 'x') {
                cout << "I'm done" << ENDLINE;
                break;
            } 
            else if (what.value == 's') {
                timer.stop();
            } 
            else if (what.value == 'r') {
                timer.start();
            } 
            else if (what.value == 'E') {
                Log::error() << "Call me inside a method" << ENDLINE;
                Log::panicOnError();
                cout << "No error" << ENDLINE;
            } 
            else {
                cout << "not x , try again" << ENDLINE;
            }
        }
    }
    catch (LogPanic& panic) {

    }
}
