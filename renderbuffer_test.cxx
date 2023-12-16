#include "renderbuffer.h"
#include <iostream>

using namespace std;

void assertBuffer(RenderBuffer &b, string expectedState) {
    string currentState = b.dumpToString();
    if (expectedState != currentState) {  
        cerr << "State does not match. EXPECTED:\n" << expectedState <<" \nCURRENT:\n" << currentState << endl;
        exit(1);
    }
}


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

void TestShouldUseInitializer() {
    RenderBuffer b(10,1,' ');

    b.writeText("HELLO",2,0);

    string line = b.asLine(0);
    if ("  HELLO   " != line) {  
        cerr << "Expected '  HELLO   ', got '" << line << "'" << endl;
        exit(1);
    }
}

void TestWriteTextShouldSpanOverLinesAndNotOverflow() {
    RenderBuffer b(5,2,' ');

    b.writeText("HELLOWORLD1234",0,0);

    string lineA = b.asLine(0);
    string lineB = b.asLine(1);
    if ("HELLO" != lineA || "WORLD" != lineB) {  
        cerr << "Expected 2 lines: HELLO and WORLD, got: '" << lineA << " and " <<lineB << endl;
        exit(1);
    }
}

void TestWriteTextToBufferShouldRespectLFandTreatTABAsSpace() {
    RenderBuffer b(5,3,'.');

    b.writeText("1st\n\t2nd\n3rd\n4th This Is Ignored",0,0);

    string lineA = b.asLine(0);
    string lineB = b.asLine(1);
    string lineC = b.asLine(2);
    if ("1st.." != lineA || " 2nd." != lineB || "3rd.." != lineC) {  
        cerr << "Expected 3 lines: to match  1st../ 2nd./3rd.., got: '" << lineA << "/" <<lineB << "/" << lineC << endl;
        exit(1);
    }
}


void TestWriteBuffer() {
    RenderBuffer b(3,3,' ');
    
    b.writeText(
        " X "
        " O "
        " X "    
    ,0,0);

    RenderBuffer overlay(2,1);
    overlay.writeText("X ", 1,0);
    

    string expectedState = 
        " X \n"
        " X \n"
        " X \n";

    assertBuffer(b, expectedState);
}

int main() {
    TestOneLineBuffer();
    TestWriteMultipleLines();
    TestShouldUseInitializer();
    TestWriteTextShouldSpanOverLinesAndNotOverflow();
    TestWriteTextToBufferShouldRespectLFandTreatTABAsSpace();
    TestWriteBuffer();
    return 0;
}