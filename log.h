#pragma once
#include <iostream>
#include <sstream>

struct LogPanic {
    std::string str;
};

class Log {
    static std::stringstream errorStream;
    public:
    
    static std::stringstream& error();
    static void panicOnError();
    static void panic();
    static void clear();

};
