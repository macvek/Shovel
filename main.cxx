#include <iostream>
#include <unistd.h>

#include "console.h"
#include "timer.h"
#include "log.h"

using namespace std;

struct MyOnTick : public TimerOnTick {
    void onTick() {
        std::cout << "On My Tick\r\n";
    }
};

int main(int argc, char** argv) {
    Console console;
    MyOnTick myOnTick;
    try {
        Timer timer(200, &myOnTick);
        console.enableRaw();
        
        char bytes[1024];
        bytes[0] = '!';
        for (;;) {
            int ret = read(STDIN_FILENO, bytes, 1024);
            char what = bytes[0];
            cout << "GOT : "<<bytes[0] << ";" << ret << "\r\n";
            if (what == 'x') {
                cout << "I'm done" << "\r\n";
                break;
            } 
            else if (what == 's') {
                timer.stop();
            } 
            else if (what == 'r') {
                timer.start();
            } 
            else if (what == 'E') {
                Log::error() << "Call me inside a method" << "\r\n";
                Log::panicOnError();
                cout << "No error\r\n";
            } 
            else {
                cout << "not x , try again" << "\r\n";
            }
        }
    }
    catch (LogPanic& panic) {

    }
}
