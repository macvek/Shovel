#include "renderbuffer.h"

RenderBuffer::RenderBuffer(int aWidth, int aHeight, char initial) : width(aWidth), height(aHeight) {
    frontBuffer.resize(aWidth * aHeight, initial);
}

void RenderBuffer::writeText(std::string text, int x, int y) {
    auto src = text.begin();
    auto dest = frontBuffer.begin();
    for (; src < text.end() && dest < frontBuffer.end(); ++src, ++dest) {
        *dest = *src;
    }
}

void RenderBuffer::fillVector(std::vector<char> &out) const {
    out = frontBuffer;
}

int RenderBuffer::getWidth() const { 
    return width;
}

int RenderBuffer::getHeight() const {
    return height;
}
