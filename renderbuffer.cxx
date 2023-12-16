#include "renderbuffer.h"

RenderBuffer::RenderBuffer(int aWidth, int aHeight, char initial) : width(aWidth), height(aHeight) {
    frontBuffer.resize(aWidth * aHeight+1, initial);
    frontBuffer[aWidth * aHeight] = 0;  // always keep extra 0 in the end so it can be dumped safely
}

void RenderBuffer::writeText(std::string text, int x, int y) {
    auto offset = xyOffset(x,y);
    
    auto src = text.begin();
    auto dest = frontBuffer.begin() + offset.ptr;

    for (; src < text.end() && dest < frontBuffer.end(); ++src, ++dest) {
        if (*src == '\t') {
            *dest = ' ';
        }
        else if (*src == '\n') {
            if (y+1 == height) {
                return;
            }
            
            writeText( std::string(src+1, text.end()), 0, y+1);
            return;
        }
        else {
            *dest = *src;
        }
    }
}

std::string RenderBuffer::asLine(int line) const {
    auto offset = xyOffset(0, line);
    return std::string(frontBuffer.data() + offset.ptr, offset.charsInLine);
}

int RenderBuffer::getWidth() const { 
    return width;
}

int RenderBuffer::getHeight() const {
    return height;
}

RenderBuffer::XYOffset RenderBuffer::xyOffset(int x, int y) const{
    int line = y < 0 ? 0 : y >= height ? height : y;
    int column = x < 0 ? 0 : x >= width ? width : x;
    return { .ptr = line * width + column, .charsInLine = width - column};
}
