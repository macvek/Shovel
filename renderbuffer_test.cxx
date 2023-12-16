#include "renderbuffer.h"
#include <iostream>

using namespace std;

void TestOneLineBuffer() {
    RenderBuffer b(11,1); // 10-chars + 1-stringend
    if (1 != b.getHeight() || 11 != b.getWidth()) {
        cerr << "Buffer should be 11x1 , got "<<b.getWidth() << "x" << b.getHeight() << endl;
        exit(1);
    }

    b.writeText("HELLOWORLD",0,0);

    std::vector<char> fill;
    b.fillVector(fill);

    std::string asString(fill.data());
    if (asString != "HELLOWORLD") { 
        cerr << "Buffer should be HELLOWORLD, got "<< asString << endl;
        exit(1);
    }
}


int main() {
    TestOneLineBuffer();
    return 0;
}