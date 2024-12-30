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

#include "frame.h"

const std::string Frame::Hash = "#### ####";
const std::string Frame::DashAndSlash = "/-\\| |\\-/";
const std::string Frame::SingleBorder = "\xDA\xC4\xBF\xB3 \xB3\xC0\xC4\xD9";
const std::string Frame::SingleHorisontalExtend = "\xC2\xC4\xC2\xB3 \xB3\xC1\xC4\xC1";
const std::string Frame::DoubleBorder = "\xC9\xCD\xBB\xBA \xBA\xC8\xCD\xBC";

static void prepareLine(const char* source, std::string& dest) {
    dest[0] = source[0];
    int limit = dest.length()-1;
    for (int i=1; i<limit; ++i) {
        dest[i] = source[1];
    }
    dest[limit] = source[2];
}

void Frame::drawFrame(RenderBuffer& where, int left, int top, int right, int bottom, std::string input) {
    /*
    a bbbbb c
    d eeeee f
    g hhhhh i
    */

    int width = 1+right - left;
    int height = 1+bottom - top;

    if (width <= 0 || height <= 0) {
        return;
    }

    const char* rawInput = input.c_str();
    const char *first = rawInput;
    const char *middle = rawInput+3;
    const char *last = middle+3;

    RenderBuffer f(width, height, 0);

    std::string line(width, ' ');
    
    prepareLine(first, line);
    f.writeText(line, 0,0);

    prepareLine(middle, line);
    for (int i=1; i< height-1;i++) {
        f.writeText(line, 0, i);
    }

    prepareLine(last, line);
    f.writeText(line, 0,height-1);
    
    where.writeView(f.view(),left,top);
}
