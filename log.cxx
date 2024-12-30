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

#ifdef BUILDONWINDOWS

#else
#include <execinfo.h> 
#endif

#include <errno.h>
#include <string.h>

#include "output.h"
#include "log.h"

#define FRAMESCOUNT 30
static void *backtraceBuffer[FRAMESCOUNT];

std::stringstream Log::errorStream;
std::ostream& Log::info() {
    return std::cout;
}

std::ostream& Log::warn() {
    return std::cout;
}

std::ostream& Log::error() {
#ifdef BUILDONWINDOWS

#else
    int nbOfTraces = backtrace((void**)&backtraceBuffer, FRAMESCOUNT);
    char **strings = backtrace_symbols(backtraceBuffer, nbOfTraces);
    if (strings == nullptr) {
        errorStream << "failed to produce backtrace: " << strerror(errno) << ENDLINE;
    }
    else {
        for (int i = 0; i < nbOfTraces; i++) {
            errorStream << strings[i] << ENDLINE;
        }
        errorStream << ENDLINE;
        free(strings);
    }

#endif
    return errorStream;
}

void Log::panicOnError() {
    if (errorStream.str().length() > 0) {
        panic();
    }
}

void Log::panic() {
    std::string message = errorStream.str();
    std::cerr << "PANIC:" << message << ENDLINE;
    LogPanic panic;
    panic.str = message;
    throw panic;
}

void Log::clear() {
    errorStream.clear();
}

void Log::panicWithErrno(std::string msg) {
    error() << msg << "; errno:" << errno << ", strerror: " << strerror(errno) << ENDLINE;
    panic();
}