#pragma once
#include <time.h>

struct TimerOnTick {
    virtual void onTick() = 0;
};

class Timer {
    const int milisecInterval;
    TimerOnTick *onTick;
    timer_t id;
    
    public:
    Timer(int aMilisecInternval, TimerOnTick* aOnTick);
    ~Timer();
    void start();
    void stop();
    void trigger();
};


