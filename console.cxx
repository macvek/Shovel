#include "console.h"
#include "log.h"
#ifdef BUILDONWINDOWS
#include "buildonwindows.h"
#else
#include <unistd.h>
#endif

Console::Console() {
#ifdef BUILDONWINDOWS
    if (FALSE == GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &initial)) {
        Log::panicWithErrno("Failed on GetConsoleMode INPUT");
    }
#else
    if (tcgetattr( STDIN_FILENO,  &initial)) {
        Log::panicWithErrno("Failed on Console constructor");
    }
#endif
    applied = initial;
}

Console::~Console() {
    restore();
}

void Console::restore() {
#ifdef BUILDONWINDOWS
    if (FALSE == SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), initial)) {
        Log::panicWithErrno("Failed on restore");
    }
#else
    if (tcsetattr( STDIN_FILENO, TCSANOW, &initial)) {
        Log::panicWithErrno("Failed on restore");
    }
#endif
}

void Console::enableRaw() {
#ifdef BUILDONWINDOWS
    applied = 0; // not features enabled
    SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), applied);
#else    
    cfmakeraw(&applied);
    applied.c_oflag |= OPOST | OCRNL;
    if (tcsetattr( STDIN_FILENO, TCSANOW, &applied)) {
        Log::panicWithErrno("Failed on enableRaw");
    }
#endif
}