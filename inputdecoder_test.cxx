#include "inputdecoder.h"
#include <iostream>

using namespace std;
AChar ascii[128];
    
void TestFeedingStandardKey() {
    InputDecoder decoder;
    AChar buffer[] = {'a','b','c'};
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

void TestFeedingAll0x20PlusCharacters() {
    InputDecoder decoder;
    AChar* buffer = ascii+0x20; //0x20 is 'space' as first of typable characters
    int size = 0x7F - 0x20; // 0x7F is excluded as it is DEL character
    
    decoder.feed(buffer, size); 
    
    for (int i=0;i<size;i++) {
        if (!decoder.canLoad()) {
            cerr << "TestFeedingStandardKey: canLoad failed on position:" << i << endl;
            exit(1);
        }

        Key k = decoder.load();
        if (k.type != STANDARD) {
            cerr << "TestFeedingStandardKey: non STANDARD char returned on position:" << i << "; "<< k.value << endl;
            exit(1);
        }

        if (k.value != buffer[i]) {
            cerr << "TestFeedingStandardKey: expected:" << buffer[i] << ", got "<< k.value << endl;
            exit(1);
        }
    }
}


int main() {
    for (int i=0;i<128;i++) {
        ascii[i] = i;
    }

    TestFeedingStandardKey();
    TestEmptyDecoder();
    TestFeedingAll0x20PlusCharacters();
}