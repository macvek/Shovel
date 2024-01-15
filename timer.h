#pragma once

#ifdef BUILDONWINDOWS

#else
#include <time.h>
#endif

struct TimerOnTick {
    virtual void onTick() = 0;
};

class Timer {
    const int milisecInterval;
    TimerOnTick *onTick;

#ifdef BUILDONWINDOWS
    HANDLE hTimer;
#else
    timer_t id;
#endif
    
    public:
    Timer(int aMilisecInternval, TimerOnTick* aOnTick);
    ~Timer();
    void start();
    void stop();
    void trigger();
};


