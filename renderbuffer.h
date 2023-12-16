#include "terminal.h"
#include <vector>

class RenderBuffer {
    struct XYOffset {
        int ptr;
        int charsInLine;
    };


    std::vector<char> frontBuffer;
    const int width;
    const int height;
    XYOffset xyOffset(int x, int y) const;

    public:
    RenderBuffer(int width, int height, char initial=0);
    void writeText(std::string text, int x, int y);
    std::string asLine(int line) const;    

    int getWidth() const;
    int getHeight() const;
};