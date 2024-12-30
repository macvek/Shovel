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

#include "terminal.h"
#include <iostream>

#ifdef BUILDONWINDOWS
#include "buildonwindows.h"
#endif


static inline std::string CSI() {
    return "\x1B[";
}

#ifdef BUILDONWINDOWS
static int hasUnicode = 0;
static BOOL CALLBACK enumCodePagesProc(LPTSTR lpCodePageString) {
    if (std::string("65001") == std::string(lpCodePageString)) {
        hasUnicode = 1;
        return FALSE;
    }
    else {
        return TRUE;
    }
}

static bool setupCodePage() {
    EnumSystemCodePagesA(enumCodePagesProc, CP_INSTALLED);
    return hasUnicode && SetConsoleOutputCP(65001);
}

#endif

Terminal::Terminal(std::ostream &aOut) : out(aOut), calculatedX(1), calculatedY(1) {
#ifdef BUILDONWINDOWS
    HANDLE outHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (FALSE == GetConsoleMode(outHandle, &initialConsoleMode)) {
        aOut << "!! Failed on GetConsoleMode OUTPUT !!";
    }

    SetConsoleMode(outHandle, initialConsoleMode | ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    if (!setupCodePage()) {
        aOut << "!! Failed to set UTF-8 console code page !!";
    }
    
#endif
}

std::string Terminal::NoEscape(std::string src) {
    std::string out;
    for (int i=0;i<src.size();i++) {
        if (src[i] == '\x1B') {
            out += '^';
        }
        else {
            out += src[i];
        }
    }

    return out;
}

void Terminal::reset() {
    out << "\x1B[0m";
}

void Terminal::foreDefault() {
    foreColor(DEFAULT);
}

void Terminal::backDefault() {
    backColor(DEFAULT);
}

void Terminal::foreColor(COLOR it) {
    out << CSI() << 30+it << "m";
}

void Terminal::backColor(COLOR it) {
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

void Terminal::flush() {
    out.flush();
}

