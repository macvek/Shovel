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

    Key keyA = decoder.load();
    Key keyB = decoder.load();
    Key keyC = decoder.load();

    if (keyA.value != 'a' || keyB.value != 'b' || keyC.value != 'c') {
        cerr << "Expected abc, got : " << (char)keyA.value << (char)keyB.value << (char)keyC.value << endl;
        exit(1);
    }

    if (decoder.canLoad()) {
        cerr << "TestFeedingStandardKey: buffer should be empty" << endl;
        exit(1);
    }
}

void TestEmptyDecoder() {
    InputDecoder decoder;
    if (decoder.canLoad()) {
        cerr << "TestEmptyDecoder: empty decoder should not load anything" << endl;
        exit(1);
    }

    Key k = decoder.load();
    if (k.type != ERROR) {
        cerr << "TestEmptyDecoder: loading on empty should return ERROR type" << endl;
        exit(1);
    }
}

int main() {
    TestFeedingStandardKey();
    TestEmptyDecoder();
}