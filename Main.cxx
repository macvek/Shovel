#include <iostream>

#include <unistd.h>
#include <signal.h>
#include <time.h>

#include "console.h"

using namespace std;

static void handler(int sig) {
    cout << "TICK \r\n";
}

int main(int argc, char** argv) {
    Console console;
    
    struct itimerspec its = {0};
    its.it_value.tv_sec =1;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec  = 0;
    its.it_interval.tv_nsec = 500000000;

    sigevent_t sev = {0};
    struct sigaction sa = { 0 };
    sa.sa_handler = handler;

    if (sigaction(SIGRTMIN, &sa, NULL) == -1){
        cout << "sigaction failed " << errno << "\r\n";
        return -1;
    }

    sev.sigev_notify = SIGEV_SIGNAL; // Linux-specific
    sev.sigev_signo = SIGRTMIN;
    
    timer_t timerId;
    if (timer_create(CLOCK_REALTIME, &sev, &timerId)) {
        cout << "timer_create failed " << errno << "\r\n";
        return 1;
    }

    if (timer_settime(timerId, 0, &its, NULL)) {
        cout << "timer_settime failed " << errno << "\r\n";
        return 1;
    }

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
