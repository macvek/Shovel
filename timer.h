#pragma once
#include <time.h>

struct TimerOnTick {
    virtual void onTick() = 0;
};

class Timer {
    timer_t id;
    TimerOnTick *onTick;
    public:
    Timer();
    ~Timer();
    void start();
    void stop();
    void trigger();
    void setOnTick(TimerOnTick* aOnTick);
};


