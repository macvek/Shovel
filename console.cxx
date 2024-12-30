/*
MIT License

Copyright (c) 2024 Maciej Aleksandrowicz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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