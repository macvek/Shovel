#include "inputdecoder.h"

static Key SPECIAL_EMPTY = {
    .value = '~',
    .type = ERROR
};

bool InputDecoder::canLoad() {
    return queue.size() > 0;
}

#define STANDARD_BEGIN ' '
#define STANDARD_END '~'
#define HEX_ENTER   0x0D
#define HEX_ESCAPE  0x1B
#define HEX_TAB     0x09

static KeyType checkType(AChar *of) {
    if (*of >= STANDARD_BEGIN && *of <= STANDARD_END) {
        return STANDARD;
    }
    else {
        return SPECIALS;
    }
}

static int decodeKey(AChar* buffer, AChar* limit, Key& k) {
    if (limit - buffer > 0) {
        if ( *buffer == HEX_ENTER) {
            k.type = ENTER;
            k.value = '~';
            return 1;
        }
        
        if ( *buffer == HEX_ESCAPE) {
            k.type = ESCAPE;
            k.value = '~';
            return 1;
        }

        if ( *buffer == HEX_TAB) {
            k.type = TAB;
            k.value = '~';
            return 1;
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