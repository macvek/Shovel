#pragma once
#include <iostream>

class Terminal {
    std::ostream &out;
    int calculatedX;
    int calculatedY;

    public:
    Terminal(std::ostream& aOut);

    void reset();
    void foreDefault();
    void foreColor(int num);
    void foreColorRGB(int r,int g, int b);

    void backDefault();
    void backColor(int num);
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

