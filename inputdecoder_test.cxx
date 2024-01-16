#include "inputdecoder.h"
#include <iostream>
#include <string.h>

using namespace std;
#ifdef BUILDONWINDOWS
int main() {
    cerr << "This test is skipped on Windows as there is no logic in reading input keys and mapping it back to pressed characters" << endl;
    return 0;
}
#else

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

void assertModifier(std::string prefix, Key& k, Modifier mod) {
    if (k.modifier != mod) {
        cerr << prefix << " expected modifier :" << mod << ", got "<< k.modifier << endl;
        exit(1);
    }
}

void assertTypesInBuffer(std::string prefix, Key& k, KeyType* types, int i) {
    if (k.type != types[i]) {
        cerr << prefix << " expected type:" << types[i] << ", got "<< (int)k.type << " on position " << i << endl;
        exit(1);
    }
}

void assertKeySequence(std::string prefix, InputDecoder& decoder, AChar *buffer, int bufferSize, KeyType *toMatch, int toMatchCount, Modifier modifier = NONE) {
    decoder.feed(buffer, bufferSize); 
    for (int i=0;i<toMatchCount;i++) {
        assertCanLoad(prefix, decoder, i);
        Key k = decoder.load();
        
        if (k.type <= SPECIALS) {
            cerr << prefix << " non SPECIAL char returned on position:" << i << "; "<< k.type << endl;
            exit(1);
        }

        assertTypesInBuffer(prefix, k, toMatch, i);
        assertModifier(prefix, k, modifier);
    }
    assertEmpty(prefix, decoder);
}

void assertKeysMatchBuffer(std::string prefix, InputDecoder& decoder, AChar *buffer, int size, KeyType expectedType, Modifier expectedModifier) {
    for (int i=0;i<size;i++) {
        assertCanLoad(prefix, decoder, i);
        Key k = decoder.load();
        
        if (k.type != expectedType) {
            cerr << prefix << " expected type " << expectedType << " got " << k.type << " char returned on position:" << i << "; "<< k.value << endl;
            exit(1);
        }

        assertCharInBuffer(prefix, k, buffer, i);
        assertModifier(prefix, k, expectedModifier);
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
    InputDecoder decoder;
    const int size = 0x1b;
    
    AChar input[size];
    AChar output[size];
    for (int i=0;i<size;i++) {
        if (i == 0 || /*ignore special chars in this test */i == '\t' || i == '\r' || i == '\n') {
            input[i] = 0;
            output[i] = ' ';    // space is returned instead of @ as it is more obvious to hit CTRL+SPACE than CTRL+@
        }
        else {
            input[i] = i;
            output[i] = i+0x40;
        }

    }
    decoder.feed(input, size); 
    assertKeysMatchBuffer(__FUNCTION__, decoder, output, size, STANDARD_MODIFIED, CTRL);
}

void TestFeedingAll0x20PlusCharactersWithMeta() {
    InputDecoder decoder;
    AChar* buffer = ascii+0x20; //0x20 is 'space' as first of typable characters
    const int size = 0x7F - 0x20; // 0x7F is excluded as it is DEL character

    AChar metaBuffer[size*2];
    for (int i=0;i<size;i++) {
        metaBuffer[i*2] = 0x1b;
        metaBuffer[i*2 +1] = buffer[i];
    }
    
    decoder.feed(metaBuffer, size*2); 
    assertKeysMatchBuffer(__FUNCTION__, decoder, buffer, size, STANDARD_MODIFIED, META);
}

void TestFeedingMultiBytePlusCtrl() {
    InputDecoder decoder;
    AChar buffer[] = {
        0x1b, 0x5b, 0x31, 0x3b, 0x35, 0x50, // CTRL+F1
        0x1b, 0x5b, 0x31, 0x3b, 0x35, 0x51, // CTRL+F2
        0x1b, 0x5b, 0x31, 0x3b, 0x35, 0x52, // CTRL+F3
        0x1b, 0x5b, 0x31, 0x3b, 0x35, 0x53, // CTRL+F4
        
        0x1b, 0x5b, 0x31, 0x35, 0x3b, 0x35, 0x7e, // CTRL+F5
        0x1b, 0x5b, 0x31, 0x37, 0x3b, 0x35, 0x7e, // CTRL+F6
        0x1b, 0x5b, 0x31, 0x38, 0x3b, 0x35, 0x7e, // CTRL+F7
        0x1b, 0x5b, 0x31, 0x39, 0x3b, 0x35, 0x7e, // CTRL+F8
        0x1b, 0x5b, 0x32, 0x30, 0x3b, 0x35, 0x7e, // CTRL+F9
        0x1b, 0x5b, 0x32, 0x31, 0x3b, 0x35, 0x7e, // CTRL+F10
        0x1b, 0x5b, 0x32, 0x33, 0x3b, 0x35, 0x7e, // CTRL+F11
        0x1b, 0x5b, 0x32, 0x34, 0x3b, 0x35, 0x7e, // CTRL+F12

        0x1b, 0x5b, 0x31, 0x3b, 0x35, 0x41, // CTRL+UP
        0x1b, 0x5b, 0x31, 0x3b, 0x35, 0x42, // CTRL+DOWN
        0x1b, 0x5b, 0x31, 0x3b, 0x35, 0x43, // CTRL+RIGHT
        0x1b, 0x5b, 0x31, 0x3b, 0x35, 0x44, // CTRL+LEFT

        0x1b, 0x5b, 0x31, 0x3b, 0x35, 0x46, // CTRL+END
        0x1b, 0x5b, 0x31, 0x3b, 0x35, 0x48, // CTRL+HOME
        0x1b, 0x5b, 0x32, 0x3b, 0x35, 0x7e, // CTRL+INSERT
        0x1b, 0x5b, 0x33, 0x3b, 0x35, 0x7e, // CTRL+DELETE
        0x1b, 0x5b, 0x35, 0x3b, 0x35, 0x7e, // CTRL+PAGEUP
        0x1b, 0x5b, 0x36, 0x3b, 0x35, 0x7e, // CTRL+PAGEDOWN
    };

    KeyType toMatch[] = {
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,

        ARROW_UP,
        ARROW_DOWN,
        ARROW_RIGHT,
        ARROW_LEFT,
        END,
        HOME,
        INSERT,
        DELETE,
        PAGEUP,
        PAGEDOWN,
    };

    assertKeySequence(__FUNCTION__, decoder, buffer, sizeof(buffer), toMatch, sizeof(toMatch)/sizeof(KeyType), CTRL);
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
    TestFeedingAll0x20PlusCharactersWithMeta();
    TestFeedingMultiBytePlusCtrl();
}

#endif