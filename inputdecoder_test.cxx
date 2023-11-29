#include "inputdecoder.h"
#include <iostream>

using namespace std;

void TestFeedingStandardKey() {
    InputDecoder decoder;
    char buffer[] = {'a','b','c'};
    decoder.feed(buffer, 3);
    if (!decoder.canLoad()) {
        cerr << "TestFeedingStandardKey: canLoad failed" << endl;
        exit(1);
    }
}


int main() {
    TestFeedingStandardKey();
    
}