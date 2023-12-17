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
    RenderBufferView(const RenderBuffer& aFrom, const int aLeft, const int aRight, const int aTop, const int aBottom);
    friend RenderBuffer;
};

struct RenderUnit {
    int top;
    int left;
    int right;
};

class RenderBuffer {
    struct XYOffset {
        int ptr;
        int charsInLine;
    };

    std::vector<char> frontBuffer;
    std::vector<TermColor> colorBuffer;
    const int width;
    const int height;
    XYOffset xyOffset(int x, int y) const;

    char transparentChar;
    public:
    RenderBuffer(int width, int height, char initial=0, bool hasColorBuffer = false);
    void writeText(std::string text, int x, int y);
    void writeView(const RenderBufferView& view, int x, int y);

    void diff(const RenderBuffer& other, std::vector<RenderUnit>& out, int threshold = 0);
    const RenderBufferView view(int left = -1, int right = -1, int top = -1, int bottom = -1) const;

    void toTerminal(Terminal& t, int posX, int posY);
    void unitsToTerminal(Terminal &t, std::vector<RenderUnit>& units, int x, int y);

    std::string asLine(int line) const;    
    std::string dumpToString(char emptyChar = ' ') const;
    
    inline int getWidth() const {
        return width;
    }
    inline int getHeight() const {
        return height;
    }

    inline void setTransparentChar(char c) {
        transparentChar = c;
    }
    
};
