#include "terminal.h"
#include <vector>

class RenderBuffer {
    std::vector<char> frontBuffer;
    const int width;
    const int height;

    public:
    RenderBuffer(int width, int height, char initial=0);
    void writeText(std::string text, int x, int y);
    void fillVector(std::vector<char> &out) const;

    int getWidth() const;
    int getHeight() const;
};