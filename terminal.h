#pragma once

class Terminal {
    
    public:
    void reset();
    void foreDefault();
    void foreColor(int num);
    void foreColorRGB(int r,int g, int b);

    void backDefault();
    void backColor(int num);
    void backColorRGB(int r,int g, int b);

};

