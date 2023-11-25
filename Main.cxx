#include <iostream>
#include <termios.h> 
#include <unistd.h>
#include <signal.h>
#include <time.h>

using namespace std;

static void handler(int sig) {
    cout << "TICK \r\n";
}

int main(int argc, char** argv) {
    struct termios origTermios, toApply;

    struct itimerspec its = {0};
    its.it_value.tv_sec =1;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec  = 1;
    its.it_interval.tv_nsec = 0;

    sigevent_t sev = {0};
    struct sigaction sa = { 0 };
    sa.sa_handler = handler;

    if (sigaction(SIGRTMIN, &sa, NULL) == -1){
        cout << "sigaction failed " << errno << endl;
        return -1;
    }

    sev.sigev_notify = SIGEV_SIGNAL; // Linux-specific
    sev.sigev_signo = SIGRTMIN;
    
    timer_t timerId;
    if (timer_create(CLOCK_REALTIME, &sev, &timerId)) {
        cout << "timer_create failed " << errno << endl;
        return 1;
    }

    if (timer_settime(timerId, 0, &its, NULL)) {
        cout << "timer_settime failed " << errno << endl;
        return 1;
    }

    tcgetattr( STDIN_FILENO,  &origTermios);
    cfmakeraw(&toApply);
    tcsetattr( STDIN_FILENO, TCSANOW, &toApply);
    char bytes[1024];
    for (;;) {
        int ret = read(STDIN_FILENO, bytes, 1024);
        char what = bytes[0];
        cout << "GOT : "<<bytes[0] << ";" << ret << endl;
        if (what == 'x') {
            cout << "I'm done" << endl;
            break;
        }
        else {
            cout << "not x , try again" << endl;
        }
    }
    
    tcsetattr( STDIN_FILENO, TCSANOW, &origTermios);

    
}
