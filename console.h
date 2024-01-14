#pragma once
#ifdef BUILDONWINDOWS
#include "buildonwindows.h"
#else
#include <termios.h> 
#endif

class Console {
#ifdef BUILDONWINDOWS
    DWORD initialInput, initialOutput;
    DWORD appliedInput, appliedOutput;
#else
    struct termios initial;
    struct termios applied;
#endif


    public:
    Console();
    ~Console();
    void enableRaw();
    void restore();
    

};