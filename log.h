#pragma once
#include <iostream>
#include <sstream>

struct LogPanic {
    std::string str;
};

class Log {
    static std::stringstream errorStream;
    public:
    
    static std::ostream& info();
    static std::ostream& error();
    static std::ostream& warn();
    static void panicOnError();
    static void panicWithErrno(std::string msg);
    static void panic();
    static void clear();

};
