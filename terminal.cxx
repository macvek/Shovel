#include "terminal.h"
#include <iostream>

static inline std::string CSI() {
    return "\x1B[";
}

Terminal::Terminal(std::ostream &aOut) : out(aOut) {

}

void Terminal::reset() {
    out << "\x1B[0m";
}

void Terminal::foreDefault() {
    foreColor(9);
}

void Terminal::backDefault() {
    backColor(9);
}

void Terminal::foreColor(int it) {
    out << CSI() << 30+it << "m";
}

void Terminal::backColor(int it) {
    out << CSI() << 40+it << "m";
}

void Terminal::foreColorRGB(int r, int g, int b) {
    out << CSI() << "38;2;"<<r<<";"<<g<<";"<<b<<"m";
}

void Terminal::backColorRGB(int r, int g, int b) {
    out << CSI() << "48;2;"<<r<<";"<<g<<";"<<b<<"m";
}

void Terminal::showCursor(bool visible) {
    out << CSI() << (visible ? "?25h" : "?25l");
}

void Terminal::moveCursor(int x, int y) {
    calculatedX += x;
    calculatedY += y;
    if (x != 0) {
        out << CSI() << (x > 0 ? x : -x) << (x > 0 ? 'C' : 'D');
    }
    if (y != 0) {
        out << CSI() << (y > 0 ? y : -y) << (y > 0 ? 'B' : 'A');
    }
}

void Terminal::placeCursor(int x, int y) {
    calculatedX = x;
    calculatedY = y;
    out << CSI() << y << ";" << x << "H";
}

void Terminal::clear() {
    out << CSI() << 2 << "J";
    placeCursor(1,1);
}
