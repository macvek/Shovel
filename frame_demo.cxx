#include <stdio.h>

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

    f.drawFrame(b, 1, 1, 21, 5, Frame::HashFrame);

    RenderBuffer scene(80,24,'.',true);

    t.clear();
  
    scene.writeView(b.view(), 0,0);
    scene.toTerminal(t,1,1);
    
    t.flush();
}
