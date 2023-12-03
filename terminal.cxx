#include "terminal.h"
#include <iostream>

void Terminal::reset() {
    std::cout << "\x1B[0m";
}

void Terminal::foreDefault() {
    foreColor(9);
}

void Terminal::backDefault() {
    backColor(9);
}

void Terminal::foreColor(int it) {
    std::cout << "\x1B[" << 30+it << "m";
}

void Terminal::backColor(int it) {
    std::cout << "\x1B[" << 40+it << "m";
}

void Terminal::foreColorRGB(int r, int g, int b) {
    std::cout << "\x1B[38;2;"<<r<<";"<<g<<";"<<b<<"m";
}

void Terminal::backColorRGB(int r, int g, int b) {
    std::cout << "\x1B[48;2;"<<r<<";"<<g<<";"<<b<<"m";
}



