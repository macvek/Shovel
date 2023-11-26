#include "timer.h"
#include <signal.h>

#include <iostream>
static void handler(int sig, siginfo_t *info, void *ucontext) {
    Timer* owner = (Timer*)info->si_value.sival_ptr;
    owner->onTick();
}

static int signalSeq = 0;
Timer::Timer() {
    if (SIGRTMIN + signalSeq >= SIGRTMAX) {
        std::cout << "timer() failed, sequence exhausted " << errno << "\r\n";
    }

    int signalNumber = SIGRTMIN + signalSeq;

    sigevent_t sev = {0};
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = signalNumber;
    sev.sigev_value.sival_ptr = this;
    
    if (timer_create(CLOCK_REALTIME, &sev, &id)) {
        std::cout << "timer_create failed " << errno << "\r\n";
        return;
    }

    struct sigaction sa = { 0 };
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handler;

    if (sigaction(signalNumber, &sa, NULL) == -1){
        std::cout << "sigaction failed " << errno << "\r\n";
        return;
    }
    ++signalSeq;
}

Timer::~Timer() {
    if (timer_delete(id)) {
        std::cout << "timer_delete failed " << errno << "\r\n";
        return;
    }
}

void Timer::start() {
    struct itimerspec its = {0};
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = 500000000;
    its.it_interval.tv_sec  = 0;
    its.it_interval.tv_nsec = 500000000;

    if (timer_settime(id, 0, &its, NULL)) {
        std::cout << "timer_settime failed " << errno << "\r\n";
        return;
    }
}

void Timer::stop() {
    struct itimerspec its = {0};
    if (timer_settime(id, 0, &its, NULL)) {
        std::cout << "timer_settime failed " << errno << "\r\n";
        return;
    }
}

void Timer::onTick() {
     std::cout << "onTICK " << id << "\r\n";
}
