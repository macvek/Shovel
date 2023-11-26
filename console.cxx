#include "console.h"
#include <unistd.h>

Console::Console() {
    tcgetattr( STDIN_FILENO,  &initial);
    applied = initial;
}

Console::~Console() {
    tcsetattr( STDIN_FILENO, TCSANOW, &initial);
}

void Console::enableRaw() {
    cfmakeraw(&applied);
    tcsetattr( STDIN_FILENO, TCSANOW, &applied);
}