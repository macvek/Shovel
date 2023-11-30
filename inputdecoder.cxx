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

struct KeySequence {
    KeyType k;
    AChar seq[8];
};

static KeySequence sequences[] = {
    { .k = INSERT,      .seq = {0x1b, 0x5b, 0x32, 0x7e, 0xFF}},
    { .k = DELETE,      .seq = {0x1b, 0x5b, 0x33, 0x7e, 0xFF}},
    { .k = PAGEUP,      .seq = {0x1b, 0x5b, 0x35, 0x7e, 0xFF}},
    { .k = PAGEDOWN,    .seq = {0x1b, 0x5b, 0x36, 0x7e, 0xFF}},

    { .k = ARROW_UP,    .seq = {0x1b, 0x5b, 0x41, 0xFF}},
    { .k = ARROW_DOWN,  .seq = {0x1b, 0x5b, 0x42, 0xFF}},
    { .k = ARROW_RIGHT, .seq = {0x1b, 0x5b, 0x43, 0xFF}},
    { .k = ARROW_LEFT,  .seq = {0x1b, 0x5b, 0x44, 0xFF}},
    { .k = END,         .seq = {0x1b, 0x5b, 0x46, 0xFF}},
    { .k = HOME,        .seq = {0x1b, 0x5b, 0x48, 0xFF}},

    { .k = F1,          .seq = {0x1b, 0x4f, 0x50, 0xFF}},
    { .k = F2,          .seq = {0x1b, 0x4f, 0x51, 0xFF}},
    { .k = F3,          .seq = {0x1b, 0x4f, 0x52, 0xFF}},
    { .k = F4,          .seq = {0x1b, 0x4f, 0x53, 0xFF}},

    { .k = ENTER,       .seq = {0x0d, 0xFF}},
    { .k = ESCAPE,      .seq = {0x1b, 0xFF}},
    { .k = TAB,         .seq = {0x09, 0xFF}},
    
    { .k = ERROR } // END OF MATCHES
};

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
    int bufferSize = limit-buffer;
    for (KeySequence* entry = sequences; entry->k != ERROR; ++entry) {
        int seqLength = -1;
        while (entry->seq[++seqLength] != 0xFF);

        if (seqLength <= bufferSize) {
            bool match = true;
            for (int i=0;i<seqLength && match; ++i) {
                match = match && buffer[i] == entry->seq[i];
            }

            if (match) {
                return asSpecial(k, entry->k, seqLength);
            }
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