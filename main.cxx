#include <iostream>

#include <unistd.h>

#include "console.h"
#include "timer.h"
using namespace std;

int main(int argc, char** argv) {
    Console console;
    Timer timer;

    timer.start();
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
        else {
            cout << "not x , try again" << "\r\n";
        }
    }

    
}
