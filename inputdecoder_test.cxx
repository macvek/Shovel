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

void assertKeySequence(std::string prefix, InputDecoder& decoder, AChar *buffer, int bufferSize, KeyType *toMatch, int toMatchCount) {
    decoder.feed(buffer, bufferSize); 
    for (int i=0;i<toMatchCount;i++) {
        assertCanLoad(prefix, decoder, i);
        Key k = decoder.load();
        
        if (k.type <= SPECIALS) {
            cerr << prefix << " non SPECIAL char returned on position:" << i << "; "<< k.type << endl;
            exit(1);
        }

        assertTypesInBuffer(prefix, k, toMatch, i);
    }
    assertEmpty(prefix, decoder);
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
        0x7F, // BACKSPACE
    };

    KeyType toMatch[] = {
        ENTER,
        ESCAPE,
        TAB,
        BACKSPACE
    };
    
    assertKeySequence(__FUNCTION__, decoder, buffer, sizeof(buffer), toMatch, sizeof(toMatch)/sizeof(KeyType));
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

    assertKeySequence(__FUNCTION__, decoder, buffer, sizeof(buffer), toMatch, sizeof(toMatch)/sizeof(KeyType));
}

void TestFeeding4BytesChars() {
    InputDecoder decoder;
    AChar buffer[] = {
        0x1b, 0x5b, 0x32, 0x7e, // INSERT
        0x1b, 0x5b, 0x33, 0x7e, // DELETE
        0x1b, 0x5b, 0x35, 0x7e, // PAGEUP
        0x1b, 0x5b, 0x36, 0x7e, // PAGEDOWN
    };

    KeyType toMatch[] = {
        INSERT,
        DELETE,
        PAGEUP,
        PAGEDOWN,
    };

    assertKeySequence(__FUNCTION__, decoder, buffer, sizeof(buffer), toMatch, sizeof(toMatch)/sizeof(KeyType));
}

void TestFeeding5BytesChars() {
    InputDecoder decoder;
    AChar buffer[] = {
        0x1b, 0x5b, 0x31, 0x35, 0x7e, // F5
        0x1b, 0x5b, 0x31, 0x37, 0x7e, // F6
        0x1b, 0x5b, 0x31, 0x38, 0x7e, // F7
        0x1b, 0x5b, 0x31, 0x39, 0x7e, // F8
        0x1b, 0x5b, 0x32, 0x30, 0x7e, // F9
        0x1b, 0x5b, 0x32, 0x31, 0x7e, // F10
        0x1b, 0x5b, 0x32, 0x33, 0x7e, // F11
        0x1b, 0x5b, 0x32, 0x34, 0x7e, // F12
    };

    KeyType toMatch[] = {
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
    };

    assertKeySequence(__FUNCTION__, decoder, buffer, sizeof(buffer), toMatch, sizeof(toMatch)/sizeof(KeyType));
}

void TestFeedingCtrlPlusBasicChars() {
    
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
    TestFeeding4BytesChars();
    TestFeeding5BytesChars();
    TestFeedingCtrlPlusBasicChars();
}