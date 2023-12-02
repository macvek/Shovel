#include "inputdecoder.h"
#include "nameenum.h"

static Key SPECIAL_EMPTY = {
    .value = '~',
    .type = ERROR
};

bool InputDecoder::canLoad() {
    return queue.size() > 0;
}

// https://upload.wikimedia.org/wikipedia/commons/5/5c/ASCII-Table-wide.pdf
#define STANDARD_MODIFIED_OFFSET 0x40
#define STANDARD_MODIFIED_START 0
#define STANDARD_MODIFIED_END 'Z'-STANDARD_MODIFIED_OFFSET


#define STANDARD_BEGIN ' '
#define STANDARD_END '~'
#define HEX_LF      0x0A
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
    Modifier m;
    KeyType k;
    AChar seq[8];
};

static KeySequence sequences[] = {
    { .m = NONE, .k = F5,          .seq = {0x1b, 0x5b, 0x31, 0x35, 0x7e, 0xFF}},
    { .m = NONE, .k = F6,          .seq = {0x1b, 0x5b, 0x31, 0x37, 0x7e, 0xFF}},
    { .m = NONE, .k = F7,          .seq = {0x1b, 0x5b, 0x31, 0x38, 0x7e, 0xFF}},
    { .m = NONE, .k = F8,          .seq = {0x1b, 0x5b, 0x31, 0x39, 0x7e, 0xFF}},
    { .m = NONE, .k = F9,          .seq = {0x1b, 0x5b, 0x32, 0x30, 0x7e, 0xFF}},
    { .m = NONE, .k = F10,         .seq = {0x1b, 0x5b, 0x32, 0x31, 0x7e, 0xFF}},
    { .m = NONE, .k = F11,         .seq = {0x1b, 0x5b, 0x32, 0x33, 0x7e, 0xFF}},
    { .m = NONE, .k = F12,         .seq = {0x1b, 0x5b, 0x32, 0x34, 0x7e, 0xFF}},
    
    { .m = NONE, .k = INSERT,      .seq = {0x1b, 0x5b, 0x32, 0x7e, 0xFF}},
    { .m = NONE, .k = DELETE,      .seq = {0x1b, 0x5b, 0x33, 0x7e, 0xFF}},
    { .m = NONE, .k = PAGEUP,      .seq = {0x1b, 0x5b, 0x35, 0x7e, 0xFF}},
    { .m = NONE, .k = PAGEDOWN,    .seq = {0x1b, 0x5b, 0x36, 0x7e, 0xFF}},

    { .m = NONE, .k = ARROW_UP,    .seq = {0x1b, 0x5b, 0x41, 0xFF}},
    { .m = NONE, .k = ARROW_DOWN,  .seq = {0x1b, 0x5b, 0x42, 0xFF}},
    { .m = NONE, .k = ARROW_RIGHT, .seq = {0x1b, 0x5b, 0x43, 0xFF}},
    { .m = NONE, .k = ARROW_LEFT,  .seq = {0x1b, 0x5b, 0x44, 0xFF}},
    { .m = NONE, .k = END,         .seq = {0x1b, 0x5b, 0x46, 0xFF}},
    { .m = NONE, .k = HOME,        .seq = {0x1b, 0x5b, 0x48, 0xFF}},

    { .m = NONE, .k = F1,          .seq = {0x1b, 0x4f, 0x50, 0xFF}},
    { .m = NONE, .k = F2,          .seq = {0x1b, 0x4f, 0x51, 0xFF}},
    { .m = NONE, .k = F3,          .seq = {0x1b, 0x4f, 0x52, 0xFF}},
    { .m = NONE, .k = F4,          .seq = {0x1b, 0x4f, 0x53, 0xFF}},

    { .m = NONE, .k = ENTER,       .seq = {0x0d, 0xFF}},
    { .m = NONE, .k = ESCAPE,      .seq = {0x1b, 0xFF}},
    { .m = NONE, .k = TAB,         .seq = {0x09, 0xFF}},
    { .m = NONE, .k = BACKSPACE,   .seq = {0x7F, 0xFF}},
    
    { .k = ERROR } // END OF MATCHES
};

std::string Key::typeName() {
    NAMEENUM(type, KeyType, ERROR);

    NAMEENUM(type, KeyType, STANDARD);
    NAMEENUM(type, KeyType, SPECIALS);
    
    NAMEENUM(type, KeyType, ENTER);
    NAMEENUM(type, KeyType, TAB);
    NAMEENUM(type, KeyType, ESCAPE);
    NAMEENUM(type, KeyType, BACKSPACE);

    NAMEENUM(type, KeyType, ARROW_UP);
    NAMEENUM(type, KeyType, ARROW_DOWN);
    NAMEENUM(type, KeyType, ARROW_RIGHT);
    NAMEENUM(type, KeyType, ARROW_LEFT);
    
    NAMEENUM(type, KeyType, END);
    NAMEENUM(type, KeyType, HOME);

    NAMEENUM(type, KeyType, F1);
    NAMEENUM(type, KeyType, F2);
    NAMEENUM(type, KeyType, F3);
    NAMEENUM(type, KeyType, F4);

    NAMEENUM(type, KeyType, F5);
    NAMEENUM(type, KeyType, F6);
    NAMEENUM(type, KeyType, F7);
    NAMEENUM(type, KeyType, F8);
    NAMEENUM(type, KeyType, F9);
    NAMEENUM(type, KeyType, F10);
    NAMEENUM(type, KeyType, F11);
    NAMEENUM(type, KeyType, F12);

    NAMEENUM(type, KeyType, INSERT);
    NAMEENUM(type, KeyType, DELETE);
    NAMEENUM(type, KeyType, PAGEUP);
    NAMEENUM(type, KeyType, PAGEDOWN);

    NAMEENUM(type, KeyType, STANDARD_MODIFIED);

    return "UnknownKeyType";
}

std::string Key::modifierName() {
    NAMEENUM(modifier, Modifier, NONE);
    NAMEENUM(modifier, Modifier, CTRL);
    NAMEENUM(modifier, Modifier, META);

    return "UnknownModifier";
}

static KeyType checkType(AChar *of) {
    if (*of >= STANDARD_MODIFIED_START && *of < STANDARD_MODIFIED_END && *of != HEX_ENTER && *of != HEX_LF && *of != HEX_TAB) {
        return STANDARD_MODIFIED;
    }
    else if (*of >= STANDARD_BEGIN && *of <= STANDARD_END) {
        return STANDARD;
    }
    else {
        return SPECIALS;
    }
}

static int asSpecial(Key& k, KeyType t, Modifier m,  int ret) {
    k.type = t;
    k.value = '~';
    k.modifier = m;
    return ret;
}

static int decodeCharSequence(AChar* buffer, AChar* limit, Key& k) {   
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
                return asSpecial(k, entry->k, entry->m, seqLength);
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
            k.modifier = NONE;
            queue.push(k);
            ptr++;
        }
        else if (type == STANDARD_MODIFIED) {
            if (*ptr == 0) {
                k.value = ' ';
            }
            else {
                k.value = *ptr + STANDARD_MODIFIED_OFFSET;
            }
            k.type = STANDARD_MODIFIED;
            k.modifier = CTRL;
            queue.push(k);
            ptr++;
        }
        else if (type == SPECIALS) {
            int offset = decodeCharSequence(ptr, end, k);
            
            // if it is decoded as pressed ESCAPE, check if following character is standard, as it might be META+key sequence
            if (k.type == ESCAPE && ptr+1 < end && checkType(ptr+1) == STANDARD) {
                k.value = *(ptr+1);
                k.type = STANDARD_MODIFIED;
                k.modifier = META;
                ptr += 2;
                queue.push(k);
            }
            else {
                ptr += offset;
                queue.push(k);
            }
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