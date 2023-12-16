#include "renderbuffer.h"
#include <iostream>

using namespace std;

void TestOneLineBuffer() {
    RenderBuffer b(10,1); // 10-chars, buffer should always keep extra 0 in the end
    if (1 != b.getHeight() || 10 != b.getWidth()) {
        cerr << "Buffer should be 10x1 , got "<<b.getWidth() << "x" << b.getHeight() << endl;
        exit(1);
    }

    b.writeText("HELLOWORLD",0,0);

    std::string asString(b.asLine(0));
    if (asString != "HELLOWORLD") { 
        cerr << "Buffer should be HELLOWORLD, got "<< asString << endl;
        exit(1);
    }
}

void TestWriteMultipleLines() {
    RenderBuffer b(5,2);

    b.writeText("HELLO",0,0);
    b.writeText("WORLD",0,1);

    string frst = b.asLine(0);
    string snd = b.asLine(1);
    if ("HELLO" != frst || "WORLD" != snd) {  
        cerr << "Multiline should return lines HELLO and WORLD, got "<< frst << ", "<< snd << endl;
        exit(1);
    }
}


int main() {
    TestOneLineBuffer();
    TestWriteMultipleLines();
    return 0;
}