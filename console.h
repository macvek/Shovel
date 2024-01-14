#pragma once
#ifdef BUILDONWINDOWS
#include "buildonwindows.h"
#else
#include <termios.h> 
#endif

class Console {
#ifdef BUILDONWINDOWS
    DWORD initial;
    DWORD applied;
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