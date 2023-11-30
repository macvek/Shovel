#include "inputdecoder.h"

static Key SPECIAL_EMPTY = {
    .value = '~',
    .type = ERROR
};

bool InputDecoder::canLoad() {
    return queue.size() > 0;
}

// https://upload.wikimedia.org/wikipedia/commons/5/5c/ASCII-Table-wide.pdf
#define STANDARD_BEGIN ' '
#define STANDARD_END '~'
#define HEX_ENTER   0x0D
#define HEX_ESCAPE  0x1B
#define HEX_TAB     0x09

#define HEX_ESCAPESEQ1 0x5B
#define HEX_ESCAPESEQ2 0x4F

#define HEX_ARROWUP 0x41
#define HEX_ARROWDOWN 0x42
#define HEX_ARROWRIGHT 0x43
#define HEX_ARROWLEFT 0x44

#define HEX_END 0x46
#define HEX_HOME 0x48

#define HEX_F1 0x50
#define HEX_F2 0x51 
#define HEX_F3 0x52
#define HEX_F4 0x53

static KeyType checkType(AChar *of) {
    if (*of >= STANDARD_BEGIN && *of <= STANDARD_END) {
        return STANDARD;
    }
    else {
        return SPECIALS;
    }
}

static int asSpecial(Key& k, KeyType t, int ret) {
    k.type = t;
    k.value = '~';
    return ret;
}

static int decodeKey(AChar* buffer, AChar* limit, Key& k) {   
    if (limit - buffer >= 3) {
        if (*buffer == HEX_ESCAPE && *(buffer+1) == HEX_ESCAPESEQ1) {
            switch( *(buffer+2) ) {
                case HEX_ARROWUP: return asSpecial(k, ARROW_UP, 3);
                case HEX_ARROWDOWN: return asSpecial(k, ARROW_DOWN, 3);
                case HEX_ARROWLEFT: return asSpecial(k, ARROW_LEFT, 3);
                case HEX_ARROWRIGHT: return asSpecial(k, ARROW_RIGHT, 3);
                case HEX_HOME: return asSpecial(k, HOME, 3);
                case HEX_END: return asSpecial(k, END, 3);
            }
        }

        if (*buffer == HEX_ESCAPE && *(buffer+1) == HEX_ESCAPESEQ2) {
            switch( *(buffer+2) ) {
                case HEX_F1: return asSpecial(k, F1, 3);
                case HEX_F2: return asSpecial(k, F2, 3);
                case HEX_F3: return asSpecial(k, F3, 3);
                case HEX_F4: return asSpecial(k, F4, 3);
            }
        }
    }
    
    if (limit - buffer >= 1) {
        switch(*buffer) {
            case HEX_ENTER: return asSpecial(k, ENTER, 1);
            case HEX_ESCAPE: return asSpecial(k, ESCAPE, 1);
            case HEX_TAB: return asSpecial(k, TAB, 1);
        }
    }

    k.type = ERROR;
    k.value = '?';
    return 1;
}



void InputDecoder::feed(AChar* ptr, int bufferSize) {
    AChar *end = ptr+bufferSize;
    Key k = {};
    while(ptr < end) {
        KeyType type = checkType(ptr);
        if (type == STANDARD) {
            k.value = *ptr;
            k.type = STANDARD;
            queue.push(k);
            ptr++;
        }
        else if (type == SPECIALS) {
            ptr += decodeKey(ptr, end, k);
            queue.push(k);
        }
    }
}

Key InputDecoder::load() {
    if (!canLoad()) {
        return SPECIAL_EMPTY;
    }
    Key ret = queue.front();
    queue.pop();
    return ret;
}