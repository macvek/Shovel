#pragma once
#include <iostream>

typedef unsigned short TermColor;

class Terminal {
    std::ostream &out;
    int calculatedX;
    int calculatedY;

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

        UNKNOWN = -1
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

