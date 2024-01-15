#ifdef BUILDONWINDOWS
#include "buildonwindows.h"
#else
#include <signal.h>
#endif

#include <iostream>

#include "timer.h"
#include "log.h"
#include "output.h"

#ifdef BUILDONWINDOWS

// https://learn.microsoft.com/en-us/windows/win32/sync/using-a-waitable-timer-with-an-asynchronous-procedure-call
// 
static void handler(
    LPVOID pointerToTimerClass,
    DWORD unused1,
    DWORD unused2
) {
    Timer* owner = (Timer*)pointerToTimerClass;
    if (owner == nullptr) {
        Log::warn() << "Triggered timer event handler with NULL pointer" << ENDLINE;
    }
    else {
        owner->trigger();
    }
}

Timer::Timer(int aMilisecInterval, TimerOnTick* aOnTick) : milisecInterval(aMilisecInterval), onTick(aOnTick) {
    hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
    if (NULL == hTimer)
    {
        Log::error() << "Failed to CreateWaitableTimer " << GetLastError() << ENDLINE;
        Log::panicOnError();
    }
}

Timer::~Timer() {
    CloseHandle(hTimer);
} 

void Timer::start() {
    LARGE_INTEGER liDueTime;
    liDueTime.QuadPart = -1 * milisecInterval * 10000; // -1 stands for relative interval

    if (false == SetWaitableTimer(hTimer, &liDueTime, milisecInterval, handler, this, 0)) {
        Log::error() << "Failed to start timer: " << GetLastError() << ENDLINE;
        Log::panicOnError();
    }
}

void Timer::stop() {
    CancelWaitableTimer(hTimer);
}


#else 
static void handler(int sig, siginfo_t *info, void *ucontext) {
    Timer* owner = (Timer*)info->si_value.sival_ptr;
    if (owner == nullptr) {
        Log::warn() << "Triggered timer event handler with NULL pointer" << ENDLINE;
    }
    else {
        owner->trigger();
    }
}

static int signalSeq = 0;
Timer::Timer(int aMilisecInternval, TimerOnTick* aOnTick) : milisecInterval(aMilisecInternval), onTick(aOnTick) {
    if (SIGRTMIN + signalSeq >= SIGRTMAX) {
        Log::panicWithErrno("Timer() failed, sequence exhausted");
    }

    int signalNumber = SIGRTMIN + signalSeq;

    sigevent_t sev = {0};
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = signalNumber;
    sev.sigev_value.sival_ptr = this;
    
    if (timer_create(CLOCK_REALTIME, &sev, &id)) {
        Log::panicWithErrno("Timer - timer_create");
    }

    struct sigaction sa = { 0 };
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handler;

    if (sigaction(signalNumber, &sa, NULL) == -1){
        Log::panicWithErrno("Timer - sigaction");
    }
    ++signalSeq;
}

Timer::~Timer() {
    if (timer_delete(id)) {
        Log::warn() << "Failed to delete timer: " << id << ENDLINE;
    }
}

void Timer::start() {
    struct itimerspec its = {0};
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = milisecInterval * 1000000;
    its.it_interval.tv_sec  = 0;
    its.it_interval.tv_nsec = its.it_value.tv_nsec;

    if (timer_settime(id, 0, &its, NULL)) {
       Log::panicWithErrno("Timer - start() failed");
    }
}

void Timer::stop() {
    struct itimerspec its = {0};
    if (timer_settime(id, 0, &its, NULL)) {
        Log::panicWithErrno("Timer - stop() failed");
    }
}

#endif

void Timer::trigger() {
    onTick->onTick();
}