#include "terminal.h"
#include <vector>

class RenderBuffer;

class RenderBufferView {
    const std::vector<char> &frontBuffer;
    const int left;
    const int right;
    const int top;
    const int bottom;

    public:
    RenderBufferView(const RenderBuffer &from, const int aLeft, const int aRight, const int aTop, const int aBottom);
    int getWidth() const;
    int getHeight() const;
    void fillLine(int line, std::vector<char>::iterator &here);
};

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

    std::string dumpToString(char emptyChar = ' ') const;

    friend RenderBufferView;
};