#include "frame.h"

static void prepareLine(char* source, std::string& dest) {
    dest[0] = source[0];
    int limit = dest.length()-1;
    for (int i=1; i<limit; ++i) {
        dest[i] = source[1];
    }
    dest[limit] = source[2];
}

void Frame::drawFrame(RenderBuffer& where, int left, int top, int right, int bottom) {
    /*
    a bbbbb c
    d eeeee f
    g hhhhh i
    */

    int width = 1+right - left;
    int height = 1+bottom - top;

    if (width <= 0 || height <= 0) {
        return;
    }

    char first[3]   = {'1','2','3'};
    char middle[3]  = {'4','5','6'};
    char last[3]    = {'7','8','9'};

    RenderBuffer f(width, height, 0);

    std::string line(width, ' ');
    
    prepareLine(first, line);
    f.writeText(line, 0,0);

    prepareLine(middle, line);
    for (int i=1; i< height-1;i++) {
        f.writeText(line, 0, i);
    }

    prepareLine(last, line);
    f.writeText(line, 0,height-1);
    
    where.writeView(f.view(),left,top);
}
