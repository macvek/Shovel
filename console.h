#pragma once
#ifdef BUILDONWINDOWS

#else
#include <termios.h> 
#endif

class Console {
#ifdef BUILDONWINDOWS
    
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