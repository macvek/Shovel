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

#include <stdio.h>
#include <string.h>
#include "frame.h"
#include "log.h"

using namespace std;

int main(int argc, char** argv) {
    Terminal t(std::cout);
    Frame f;
    RenderBuffer b(23,7, 0, true);
    b.writeText(
        "@x                   x@"
        "x1                   2x"
        "                       "
        "                       "
        "                       "
        "x4                   3x"
        "@x                   x@",
    0,0);

    f.drawFrame(b, 1, 1, 21, 5,     Frame::Hash);
    f.drawFrame(b, 2, 2, 4, 4,      Frame::DashAndSlash);
    f.drawFrame(b, 5, 2, 7, 4,      Frame::SingleBorder);
    f.drawFrame(b, 8, 2, 11, 4,     Frame::DoubleBorder);

    SpecialCharsMap base = SpecialCharsMapFactory::create();
    
    RenderBuffer scene(80,24,'.',true, base);

    t.clear();
  
    scene.writeView(b.view(), 0,0);
    scene.toTerminal(t,1,1);
    
    t.flush();
}
