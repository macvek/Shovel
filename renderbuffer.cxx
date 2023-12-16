#include "renderbuffer.h"

RenderBufferView::RenderBufferView(const RenderBuffer& aFrom, const int aLeft, const int aRight, const int aTop, const int aBottom) 
    :from(aFrom), left(aLeft), right(aRight), top(aTop), bottom(aBottom) {
}

const RenderBufferView RenderBuffer::view(int left, int right, int top, int bottom) const{
    if (left < 0) left = 0;
    if (right < 0) right = width;
    if (top < 0) top = 0;
    if (bottom < 0) bottom = height;
    
    if (right > width) right = width;
    if (bottom > height) bottom = height;

    if (left >= right || top >= bottom) {
        return RenderBufferView(*this, 0, 0, 0, 0);
    }

    return RenderBufferView(*this, left, right, top, bottom);
}

struct PaddedAxis {
    int here;
    int srcOffset;
    int srcLimit;
};

static struct PaddedAxis paddAxis(int input, int limit, int viewOffset, int viewLimit) {
    PaddedAxis ret;
    
    ret.srcLimit = viewLimit;
    if (input < 0) {
        ret.here = 0;
        ret.srcOffset = viewOffset - input;
    }
    else {
        ret.here = input;
        ret.srcOffset = viewOffset;
    }
    if ( (ret.srcLimit - ret.srcOffset) + ret.here > limit) {
        ret.srcLimit = limit - ret.here + ret.srcOffset;
    }

    return ret;
}

void RenderBuffer::writeView(const RenderBufferView& view, int x, int y) {
    if (x >= width || y >= height) return;

    auto paddX = paddAxis(x, width, view.left, view.right);    
    auto paddY = paddAxis(y, height, view.top, view.bottom);

    int lineLength = paddX.srcLimit - paddX.srcOffset;
    int lines = paddY.srcLimit - paddY.srcOffset;

    for (int l=0; l<lines; ++l) {
        auto offsetInView = view.from.xyOffset(paddX.srcOffset, paddY.srcOffset + l);
        auto srcStart = view.from.frontBuffer.begin() + offsetInView.ptr;
        auto srcEnd = srcStart + lineLength;

        auto offsetInHere = xyOffset(paddX.here, paddY.here + l);
        auto hereStart = frontBuffer.begin() + offsetInHere.ptr;

        for (; srcStart < srcEnd; ++srcStart, ++hereStart) {
            *hereStart = *srcStart;
        }
    } 
}

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

std::string RenderBuffer::dumpToString(char emptyChar) const {
    std::string ret;
    for (int i=0;i<height;i++) {
        ret += asLine(i) + "\n";
    }

    return ret;
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

#include <iostream>
void RenderBuffer::diff(const RenderBuffer& other, std::vector<RenderDiffUnit>& out) {
    if (other.width != width || other.height != height) {
        return;
    }

    auto herePtr = frontBuffer.cbegin();
    auto otherPtr = other.frontBuffer.cbegin();
    RenderDiffUnit unit;

    for (int line=0;line<height; ++line) {
        int changeIdx = -1;
        for (int i=0;i<width; ++i, ++herePtr, ++otherPtr) {
            if (-1 == changeIdx && *herePtr != *otherPtr) {
                changeIdx = i;
            }
            else if ( -1 != changeIdx && *herePtr == *otherPtr) {
                unit.left = changeIdx;
                unit.right = i;
                unit.top = line;
                out.push_back(unit);
                changeIdx = -1;
            }
        }

        if (changeIdx != -1) {
            unit.left = changeIdx;
            unit.right = width;
            unit.top = line;
            out.push_back(unit);
        }
    }
}