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
    f.drawFrame(b, 5, 2, 7, 4,      Frame::Window);

    SpecialCharsMap base = SpecialCharsMapFactory::create();
    
    RenderBuffer scene(80,24,'.',true, base);

    t.clear();
  
    scene.writeView(b.view(), 0,0);
    scene.toTerminal(t,1,1);
    
    t.flush();
}
