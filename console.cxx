#include "console.h"
#include "log.h"
#include <unistd.h>

Console::Console() {
    if (tcgetattr( STDIN_FILENO,  &initial)) {
        Log::panicWithErrno("Failed on Console constructor");
    }
    applied = initial;
}

Console::~Console() {
    restore();
}

void Console::restore() {
    if (tcsetattr( STDIN_FILENO, TCSANOW, &initial)) {
        Log::panicWithErrno("Failed on restore");
    }
}

void Console::enableRaw() {
    cfmakeraw(&applied);
    applied.c_oflag |= OPOST | OCRNL;
    if (tcsetattr( STDIN_FILENO, TCSANOW, &applied)) {
        Log::panicWithErrno("Failed on enableRaw");
    }
}