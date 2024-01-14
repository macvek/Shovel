#include "console.h"
#include "log.h"
#ifdef BUILDONWINDOWS
#include "buildonwindows.h"
#else
#include <unistd.h>
#endif

Console::Console() {
#ifdef BUILDONWINDOWS
    if (FALSE == GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &initialInput)) {
        Log::panicWithErrno("Failed on GetConsoleMode INPUT");
    }
    HANDLE outHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (FALSE == GetConsoleMode(outHandle, &initialOutput)) {
        Log::panicWithErrno("Failed on GetConsoleMode OUTPUT");
    }
    
    appliedInput = initialInput;
    appliedOutput = initialOutput | ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING;

    SetConsoleMode(outHandle, appliedOutput);
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
    SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), initialInput);
    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), initialOutput);
#else
    if (tcsetattr( STDIN_FILENO, TCSANOW, &initial)) {
        Log::panicWithErrno("Failed on restore");
    }
#endif
}

void Console::enableRaw() {
#ifdef BUILDONWINDOWS
    appliedInput = 0; // not features enabled
    SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), appliedInput);
#else    
    cfmakeraw(&applied);
    applied.c_oflag |= OPOST | OCRNL;
    if (tcsetattr( STDIN_FILENO, TCSANOW, &applied)) {
        Log::panicWithErrno("Failed on enableRaw");
    }
#endif
}