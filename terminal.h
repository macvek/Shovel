#pragma once
#include <iostream>

class Terminal {
    std::ostream &out;

    public:
    Terminal(std::ostream &aOut);
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
};

