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

void assertCharInBuffer(std::string prefix, Key& k, AChar* values, int i) {
    if (k.value != values[i]) {
        cerr << prefix << " expected:" << (int)values[i] << ", got "<< (int)k.value << endl;
        exit(1);
    }
}

void assertTypesInBuffer(std::string prefix, Key& k, KeyType* types, int i) {
    if (k.type != types[i]) {
        cerr << prefix << " expected type:" << types[i] << ", got "<< (int)k.type << endl;
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
    AChar buffer[] = {
        0x0D, // ENTER,
        0x1B, // ESCAPE,
        0x09, // TAB,
    };

    KeyType toMatch[] = {
        ENTER,
        ESCAPE,
        TAB
    };
    
    decoder.feed(buffer, sizeof(buffer)); 
    int toMatchCount = (sizeof(toMatch)/sizeof(KeyType));
    for (int i=0;i<toMatchCount;i++) {
        assertCanLoad(__FUNCTION__, decoder, i);
        Key k = decoder.load();
        
        if (k.type <= SPECIALS) {
            cerr << __FUNCTION__ << " non SPECIAL char returned on position:" << i << "; "<< k.type << endl;
            exit(1);
        }

        assertTypesInBuffer(__FUNCTION__, k, toMatch, i);
    }
    assertEmpty(__FUNCTION__, decoder);
}

void TestFeeding3BytesChars() {
    InputDecoder decoder;
    AChar buffer[] = {
        0x1b, 0x5b, 0x41, // ARROW_UP
        0x1b, 0x5b, 0x42, // ARROW_DOWN
        0x1b, 0x5b, 0x43, // ARROW_RIGHT
        0x1b, 0x5b, 0x44, // ARROW_LEFT
                          // ?        
        0x1b, 0x5b, 0x46, // END
                          // ?
        0x1b, 0x5b, 0x48, // HOME
                          // ?
        0x1b, 0x4f, 0x50, // F1
        0x1b, 0x4f, 0x51, // F2
        0x1b, 0x4f, 0x52, // F3
        0x1b, 0x4f, 0x53, // F4
    };

    KeyType toMatch[] = {
        ARROW_UP,
        ARROW_DOWN,
        ARROW_RIGHT,
        ARROW_LEFT,
        END,
        HOME,
        F1,
        F2,
        F3,
        F4
    };

    decoder.feed(buffer, sizeof(buffer)); 
    int toMatchCount = (sizeof(toMatch)/sizeof(KeyType));
    for (int i=0;i<toMatchCount;i++) {
        assertCanLoad(__FUNCTION__, decoder, i);
        Key k = decoder.load();
        
        if (k.type <= SPECIALS) {
            cerr << __FUNCTION__ << " non SPECIAL char returned on position:" << i << "; "<< k.type << endl;
            exit(1);
        }

        assertTypesInBuffer(__FUNCTION__, k, toMatch, i);
    }
    assertEmpty(__FUNCTION__, decoder);
}


int main() {
    for (int i=0;i<128;i++) {
        ascii[i] = i;
    }

    TestFeedingStandardKey();
    TestEmptyDecoder();
    TestFeedingAll0x20PlusCharacters();
    TestFeedingSpecialOneChars();
    TestFeeding3BytesChars();
}