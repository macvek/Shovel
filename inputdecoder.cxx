#include "inputdecoder.h"

static Key SPECIAL_EMPTY = {
    .value = '~'
};

bool InputDecoder::canLoad() {
    return queue.size() > 0;
}

void InputDecoder::feed(char* ptr, int bufferSize) {
    char *end = ptr+bufferSize;
    Key k;
    while(ptr < end) {
        k.value = *ptr;
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