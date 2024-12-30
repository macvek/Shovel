/*
MIT License

Copyright (c) 2024 Maciej Aleksandrowicz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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

        auto viewIdx = offsetInView.ptr;
        auto viewEnd = viewIdx + lineLength;

        auto offsetInHere = xyOffset(paddX.here, paddY.here + l);
        auto hereIdx = offsetInHere.ptr;

        for (; viewIdx < viewEnd; ++viewIdx, ++hereIdx) {
            auto valueToCopy = view.from.frontBuffer[viewIdx];
            
            if (valueToCopy != view.from.transparentChar) {
                frontBuffer[hereIdx] = valueToCopy;

                if (!colorBuffer.empty() && !view.from.colorBuffer.empty()) {
                    colorBuffer[hereIdx] = view.from.colorBuffer[viewIdx];
                }
            }
        }
    }
}

RenderBuffer::RenderBuffer(int aWidth, int aHeight, char initial, bool hasColorBuffer) : RenderBuffer(aWidth, aHeight, initial, hasColorBuffer, SpecialCharsMap()) {
}

RenderBuffer::RenderBuffer(int aWidth, int aHeight, char initial, bool hasColorBuffer, SpecialCharsMap aSpecialChars) : width(aWidth), height(aHeight), transparentChar(0), specialChars(aSpecialChars) {
    int size = aWidth * aHeight+1;
    frontBuffer.resize(size, initial);
    frontBuffer[aWidth * aHeight] = 0;  // always keep extra 0 in the end so it can be dumped safely
    if (hasColorBuffer) {
        colorBuffer.resize(size, Terminal::MakeColor(Terminal::DEFAULT, Terminal::DEFAULT));
    }
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

void RenderBuffer::writeColorLine(int x, int y, int len, TermColor color) {
    if (colorBuffer.empty()) return;    
    auto offset = xyOffset(x,y);
    auto here = colorBuffer.begin()+offset.ptr;

    auto end = here + len < colorBuffer.end() ? here + len : colorBuffer.end();
   
    for (; here < end; ++here ) *here = color;
}

std::string RenderBuffer::asLine(int line) const {
    auto offset = xyOffset(0, line);
    return std::string(frontBuffer.data() + offset.ptr, offset.charsInLine);
}

std::vector<TermColor> RenderBuffer::asColorLine(int line) const {
    std::vector<TermColor> ret;
    if (!colorBuffer.empty()) {
        auto offset = xyOffset(0, line);
        auto here = colorBuffer.cbegin() + offset.ptr;
        auto end = here + offset.charsInLine;
        for ( ;here<end; ++here) {
            ret.push_back(*here);
        }
    }
    return ret;
}

std::string RenderBuffer::dumpToColoredString(int line) const {
    return "";
}

std::string RenderBuffer::dumpToString(char emptyChar) const {
    std::string ret;
    for (int i=0;i<height;i++) {
        ret += asLine(i) + "\n";
    }

    return ret;
}

RenderBuffer::XYOffset RenderBuffer::xyOffset(int x, int y) const{
    int line = y < 0 ? 0 : y >= height ? height : y;
    int column = x < 0 ? 0 : x >= width ? width : x;
    return { .ptr = line * width + column, .charsInLine = width - column};
}

void RenderBuffer::diff(const RenderBuffer& other, std::vector<RenderUnit>& out, int threshold) {
    if (other.width != width || other.height != height) {
        return;
    }
    
    auto idx = 0;
    RenderUnit unit;

    int noise = 0;
    for (int line=0;line<height; ++line) {
        int changeIdx = -1;
        for (int i=0;i<width; ++i, ++idx) {
            if ( frontBuffer[idx] != other.frontBuffer[idx] || 
                (!colorBuffer.empty() && !other.colorBuffer.empty() && colorBuffer[idx] != other.colorBuffer[idx])) {
                
                if (-1 == changeIdx) {
                    changeIdx = i;
                }

                noise = threshold;
            }
            else if ( -1 != changeIdx) {
                if (0 == noise) {
                    unit.left = changeIdx;
                    unit.right = i - threshold;
                    unit.top = line;
                    out.push_back(unit);
                    changeIdx = -1;
                }
                else {
                    --noise;
                }
            }
        }

        if (changeIdx != -1) {
            unit.left = changeIdx;
            unit.right = width - (threshold - noise);
            unit.top = line;
            out.push_back(unit);
        }
    }
}

void RenderBuffer::toTerminal(Terminal& t, int terminalPosX, int terminalPosY, bool useColor) {
    TermColor initial = Terminal::MakeColor(Terminal::DEFAULT, Terminal::DEFAULT);
    TermColor currentColor = initial;
    
    int offset = 0;
    for (int line = 0; line<height; line++) {
        currentColor = fragmentToTerminal(t, terminalPosX, terminalPosY+line, offset, offset+width, currentColor, useColor);
        offset += width;
    }

    if (currentColor != initial) {
        updateColor(t, currentColor, initial);
    }
}

void RenderBuffer::unitsToTerminal(Terminal &t, std::vector<RenderUnit>& units, int x, int y, bool useColor) {
    TermColor initial = Terminal::MakeColor(Terminal::DEFAULT, Terminal::DEFAULT);
    TermColor currentColor = initial;
    
    for (auto ptr = units.cbegin(); ptr != units.cend(); ++ptr) {
        auto offset = xyOffset(ptr->left, ptr->top);
        auto offsetEnd = offset.ptr + ptr->right - ptr->left;
        currentColor = fragmentToTerminal(t, x + ptr->left, y + ptr->top, offset.ptr, offsetEnd, currentColor, useColor);
    }

    if (currentColor != initial) {
        updateColor(t, currentColor, initial);
    }
}

void RenderBuffer::updateColor(Terminal &t, TermColor oldColor, TermColor newColor) {
    Terminal::COLOR loadedColor = Terminal::ToForeColor(newColor);
    if (loadedColor != Terminal::ToForeColor(oldColor)) {
        t.foreColor(loadedColor);
    }

    loadedColor = Terminal::ToBackColor(newColor);
    if (loadedColor != Terminal::ToBackColor(oldColor)) {
        t.backColor(loadedColor);
    }
}

TermColor RenderBuffer::fragmentToTerminal(Terminal &t, int terminalX, int terminalY, int offset, int offsetEnd, TermColor currentColor, bool useColor) {
    t.placeCursor(terminalX,terminalY);

    for (;offset < offsetEnd ; ++offset) {
        if (useColor && !colorBuffer.empty() && currentColor != colorBuffer[offset]) {
            updateColor(t, currentColor, colorBuffer[offset]);
            currentColor = colorBuffer[offset];
        }
        char renderMe = frontBuffer[offset];
        if (renderMe >= 0 || specialChars.empty()) {
            t.stream() << renderMe;
        }
        else {
            auto found = specialChars.find( (unsigned char)renderMe );
            if (found != specialChars.end()) {
                t.stream() << found->second;
            }
            else {
                t.stream() << renderMe;
            }
        }
    }

    return currentColor;
}

bool RenderBuffer::copyFrom(const RenderBuffer &other) {
    if ( this == &other ) {
        return true;
    }

    if (width != other.width || height != other.height) {
        return false;
    }

    frontBuffer = other.frontBuffer;
    colorBuffer = other.colorBuffer;

    return true;
}
