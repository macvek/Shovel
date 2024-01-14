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