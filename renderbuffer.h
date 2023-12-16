#include "terminal.h"
#include <vector>

class RenderBuffer;

struct RenderBufferView {
    const RenderBuffer &from;
    const int left;
    const int right;
    const int top;
    const int bottom;

    private:
    RenderBufferView(const RenderBuffer &aFrom, const int aLeft, const int aRight, const int aTop, const int aBottom);
    friend RenderBuffer;
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
    void writeView(const RenderBufferView& view, int x, int y);

    std::string asLine(int line) const;    
    int getWidth() const;
    int getHeight() const;

    std::string dumpToString(char emptyChar = ' ') const;

    const RenderBufferView view(int left = -1, int right = -1, int top = -1, int bottom = -1) const;
};
