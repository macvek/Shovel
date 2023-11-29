#include "inputdecoder.h"
#include <iostream>

using namespace std;
AChar ascii[128];

void assertCanLoad(std::string prefix, InputDecoder& decoder, int i) {
    if (!decoder.canLoad()) {
        cerr << prefix << " canLoad failed on position:" << i << endl;
        exit(1);
    }
}

void assertEmpty(std::string prefix, InputDecoder& decoder) {
    if (decoder.canLoad()) {
        cerr << prefix << " assertEmpty failed" << endl;
        exit(1);
    }
}

void assertCharInBuffer(std::string prefix, Key& k, AChar* buffer, int i) {
    if (k.value != buffer[i]) {
        cerr << prefix << " expected:" << buffer[i] << ", got "<< k.value << endl;
        exit(1);
    }
}

void TestFeedingStandardKey() {
    InputDecoder decoder;
    AChar buffer[] = {'a','b','c'};
    decoder.feed(buffer, 3);
    
    assertCanLoad(__FUNCTION__, decoder, 0);

    Key keyA = decoder.load();
    Key keyB = decoder.load();
    Key keyC = decoder.load();

    if (keyA.value != 'a' || keyB.value != 'b' || keyC.value != 'c') {
        cerr << __FUNCTION__ << " Expected abc, got : " << (char)keyA.value << (char)keyB.value << (char)keyC.value << endl;
        exit(1);
    }

    assertEmpty(__FUNCTION__, decoder);
}

void TestEmptyDecoder() {
    InputDecoder decoder;
    assertEmpty(__FUNCTION__, decoder);

    Key k = decoder.load();
    if (k.type != ERROR) {
        cerr << __FUNCTION__ << " loading on empty should return ERROR type" << endl;
        exit(1);
    }
}

void TestFeedingAll0x20PlusCharacters() {
    InputDecoder decoder;
    AChar* buffer = ascii+0x20; //0x20 is 'space' as first of typable characters
    int size = 0x7F - 0x20; // 0x7F is excluded as it is DEL character
    
    decoder.feed(buffer, size); 
    
    for (int i=0;i<size;i++) {
        assertCanLoad(__FUNCTION__, decoder, i);
        Key k = decoder.load();
        
        if (k.type != STANDARD) {
            cerr << __FUNCTION__ << " non STANDARD char returned on position:" << i << "; "<< k.value << endl;
            exit(1);
        }

        assertCharInBuffer(__FUNCTION__, k, buffer, i);
    }
    assertEmpty(__FUNCTION__, decoder);
}

void TestFeedingSpecialOneChars() {
    InputDecoder decoder;
    AChar* buffer = ascii+0x20; //0x20 is 'space' as first of typable characters
    int size = 0x7F - 0x20; // 0x7F is excluded as it is DEL character
    
    decoder.feed(buffer, size); 
    
    for (int i=0;i<size;i++) {
        assertCanLoad(__FUNCTION__, decoder, i);

        Key k = decoder.load();
        
        if (k.type != STANDARD) {
            cerr << __FUNCTION__ << " non STANDARD char returned on position:" << i << "; "<< k.value << endl;
            exit(1);
        }

        assertCharInBuffer(__FUNCTION__, k, buffer, i);
    }
}


int main() {
    for (int i=0;i<128;i++) {
        ascii[i] = i;
    }

    TestFeedingStandardKey();
    TestEmptyDecoder();
    TestFeedingAll0x20PlusCharacters();
    TestFeedingSpecialOneChars();
}