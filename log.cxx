#include "log.h"
#include <execinfo.h> 
#include <errno.h>
#include <string.h>

#define FRAMESCOUNT 100
static void *backtraceBuffer[FRAMESCOUNT];

std::stringstream Log::errorStream;
std::stringstream& Log::error() {
    int nbOfTraces = backtrace((void**)&backtraceBuffer, FRAMESCOUNT);
    char **strings = backtrace_symbols(backtraceBuffer, nbOfTraces);
    if (strings == nullptr) {
        errorStream << "failed to produce backtrace: " << strerror(errno) << "\r\n";
    }
    else {
        for (int i = 0; i < nbOfTraces; i++) {
            errorStream << strings[i] << "\r\n";
        }
        errorStream << "\r\n";
    }
    free(strings);
    return errorStream;
}

void Log::panicOnError() {
    if (Log::errorStream.str().length() > 0) {
        panic();
    }
}

void Log::panic() {
    std::string message = errorStream.str();
    std::cerr << "PANIC:" << message << "\r\n";
    LogPanic panic;
    panic.str = message;
    throw panic;
}

void Log::clear() {
    errorStream.clear();
}