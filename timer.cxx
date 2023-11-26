#include "timer.h"

#include <signal.h>
#include <time.h>

#include <iostream>
static void handler(int sig) {
    std::cout << "TICK \r\n";
}

void Timer::start() {
    struct itimerspec its = {0};
    its.it_value.tv_sec =1;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec  = 0;
    its.it_interval.tv_nsec = 500000000;

    sigevent_t sev = {0};
    struct sigaction sa = { 0 };
    sa.sa_handler = handler;

    if (sigaction(SIGRTMIN, &sa, NULL) == -1){
        std::cout << "sigaction failed " << errno << "\r\n";
        return;
    }

    sev.sigev_notify = SIGEV_SIGNAL; // Linux-specific
    sev.sigev_signo = SIGRTMIN;

    timer_t timerId;
    if (timer_create(CLOCK_REALTIME, &sev, &timerId)) {
        std::cout << "timer_create failed " << errno << "\r\n";
        return;
    }

    if (timer_settime(timerId, 0, &its, NULL)) {
        std::cout << "timer_settime failed " << errno << "\r\n";
        return;
    }
}

void Timer::stop() {

}

