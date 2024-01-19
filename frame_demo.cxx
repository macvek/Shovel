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

    std::string MappedWindow = "\x81\x82\x83\x84 \x84\x85\x82\x86";
    
    f.drawFrame(b, 1, 1, 21, 5,     Frame::Hash);
    f.drawFrame(b, 2, 2, 4, 4,      Frame::DashAndSlash);
    f.drawFrame(b, 5, 2, 7, 4,      MappedWindow);

    SpecialCharsMap utf8Frame;
    utf8Frame[0x81] = "┌";
    utf8Frame[0x82] = "─";
    utf8Frame[0x83] = "┐";
    utf8Frame[0x84] = "│";
    utf8Frame[0x85] = "└";
    utf8Frame[0x86] = "┘";
    
    RenderBuffer scene(80,24,'.',true, utf8Frame);

    t.clear();
  
    scene.writeView(b.view(), 0,0);
    scene.toTerminal(t,1,1);
    
    t.flush();
}
