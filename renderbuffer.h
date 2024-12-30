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

#pragma once

#include "terminal.h"
#include "specialcharsmap.h"
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

    const int width;
    const int height;
    const SpecialCharsMap specialChars;

    std::vector<char> frontBuffer;
    std::vector<TermColor> colorBuffer;
    
    XYOffset xyOffset(int x, int y) const;
    TermColor fragmentToTerminal(Terminal &t, int terminalX, int terminalY, int offset, int offsetEnd, TermColor currentColor, bool useColor);
    void updateColor(Terminal &t, TermColor oldColor, TermColor newColor);

    char transparentChar;

    public:
    RenderBuffer(int width, int height, char initial=0, bool hasColorBuffer = false);
    RenderBuffer(int width, int height, char initial, bool hasColorBuffer, SpecialCharsMap aSpecialChars);
    void writeText(std::string text, int x, int y);
    void writeView(const RenderBufferView& view, int x, int y);
    void writeColorLine(int x, int y, int len, TermColor color);

    void diff(const RenderBuffer& other, std::vector<RenderUnit>& out, int threshold = 0);
    const RenderBufferView view(int left = -1, int right = -1, int top = -1, int bottom = -1) const;

    void toTerminal(Terminal& t, int posX, int posY, bool useColor=true);
    void unitsToTerminal(Terminal &t, std::vector<RenderUnit>& units, int x, int y, bool useColor = true);

    std::string asLine(int line) const;
    std::string dumpToString(char emptyChar = ' ') const;

    std::vector<TermColor> asColorLine(int line) const;
    std::string dumpToColoredString(int line) const;
    
    inline int getWidth() const {
        return width;
    }
    inline int getHeight() const {
        return height;
    }

    inline void setTransparentChar(char c) {
        transparentChar = c;
    }

    bool copyFrom(const RenderBuffer &other);
    
};
