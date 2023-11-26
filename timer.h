#pragma once
#include <time.h>

class Timer {
    timer_t id;
    public:
    Timer();
    ~Timer();
    void start();
    void stop();
    void onTick();
};