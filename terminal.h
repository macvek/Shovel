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

#pragma once
#include <iostream>

#ifdef BUILDONWINDOWS
#include "buildonwindows.h"
#endif

typedef unsigned short TermColor;

class Terminal {
    std::ostream &out;
    int calculatedX;
    int calculatedY;

#ifdef BUILDONWINDOWS
    DWORD initialConsoleMode;
#endif

    public:
    Terminal(std::ostream& aOut);
    static std::string NoEscape(std::string);
    enum COLOR {
        BLACK = 0,
        RED,
        GREEN,
        YELLOW,
        BLUE,
        MAGENTA,
        CYAN,
        WHITE,
        DEFAULT=9,

        BRIGHT_BLACK = 60,
        BRIGHT_RED,
        BRIGHT_GREEN,
        BRIGHT_YELLOW,
        BRIGHT_BLUE,
        BRIGHT_MAGENTA,
        BRIGHT_CYAN,
        BRIGHT_WHITE,

        UNKNOWN = 255
    };

    static inline TermColor MakeColor(COLOR foreground, COLOR background) {
        return foreground | (background << 8);
    }

    static inline COLOR ToForeColor(TermColor combined) {
        return (COLOR)(combined & 0xFF);
    }

    static inline COLOR ToBackColor(int combined) {
        return (COLOR)(combined >> 8 & 0xFF);
    }

    void reset();
    void foreDefault();
    void foreColor(COLOR num);
    void foreColorRGB(int r,int g, int b);

    void backDefault();
    void backColor(COLOR num);
    void backColorRGB(int r,int g, int b);

    void moveCursor(int x, int y);
    void placeCursor(int x, int y);
    void clear();
    void flush();

    void showCursor(bool visibile);

    inline int getCalculatedX() const { return calculatedX;} 
    inline int getCalculatedY() const { return calculatedY;} 

    inline std::ostream& stream() const {
         return out;
    }
};

