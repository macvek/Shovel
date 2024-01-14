#include "console.h"
#include "log.h"
#ifdef BUILDONWINDOWS

#else
#include <unistd.h>
#endif

Console::Console() {
#ifdef BUILDONWINDOWS

#else
    if (tcgetattr( STDIN_FILENO,  &initial)) {
        Log::panicWithErrno("Failed on Console constructor");
    }
    applied = initial;
#endif
}

Console::~Console() {
    restore();
}

void Console::restore() {
#ifdef BUILDONWINDOWS

#else
    if (tcsetattr( STDIN_FILENO, TCSANOW, &initial)) {
        Log::panicWithErrno("Failed on restore");
    }
#endif
}

void Console::enableRaw() {
#ifdef BUILDONWINDOWS

#else    
    cfmakeraw(&applied);
    applied.c_oflag |= OPOST | OCRNL;
    if (tcsetattr( STDIN_FILENO, TCSANOW, &applied)) {
        Log::panicWithErrno("Failed on enableRaw");
    }
#endif
}