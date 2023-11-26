#pragma once
#include <termios.h> 

class Console {
    struct termios initial;
    struct termios applied;

    public:
    Console();
    ~Console();
    void enableRaw();
    

};