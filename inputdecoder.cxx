#include "inputdecoder.h"

static Key SPECIAL_EMPTY = {
    .value = '~',
    .type = ERROR
};

bool InputDecoder::canLoad() {
    return queue.size() > 0;
}

void InputDecoder::feed(AChar* ptr, int bufferSize) {
    AChar *end = ptr+bufferSize;
    Key k;
    while(ptr < end) {
        k.value = *ptr;
        k.type = STANDARD;
        ptr++;

        queue.push(k);
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