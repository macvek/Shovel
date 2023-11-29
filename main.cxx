#include "console.h"
#include "timer.h"
#include "log.h"
#include "input.h"
#include "output.h"
#include <stdio.h>
using namespace std;

struct MyOnTick : public TimerOnTick {
    void onTick() {
        std::cout << "On My Tick" << CRLF;
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
            char what = input.getKey();
            printf("Got 0x%x;%i;%c\r\n", what,what,what);

            if (what == 'x') {
                cout << "I'm done" << CRLF;
                break;
            } 
            else if (what == 's') {
                timer.stop();
            } 
            else if (what == 'r') {
                timer.start();
            } 
            else if (what == 'E') {
                Log::error() << "Call me inside a method" << CRLF;
                Log::panicOnError();
                cout << "No error" << CRLF;
            } 
            else {
                cout << "not x , try again" << CRLF;
            }
        }
    }
    catch (LogPanic& panic) {

    }
}
