/*
MIT License

Copyright (c) 2024 Maciej Aleksandrowicz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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
#define STANDARD_MODIFIED_END 1+'Z'-STANDARD_MODIFIED_OFFSET 


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
    { .m = CTRL, .k = F1,          .seq = {0x1b, 0x5b, 0x31, 0x3b, 0x35, 0x50, (char)0xFF}}, 
    { .m = CTRL, .k = F2,          .seq = {0x1b, 0x5b, 0x31, 0x3b, 0x35, 0x51, (char)0xFF}},
    { .m = CTRL, .k = F3,          .seq = {0x1b, 0x5b, 0x31, 0x3b, 0x35, 0x52, (char)0xFF}},
    { .m = CTRL, .k = F4,          .seq = {0x1b, 0x5b, 0x31, 0x3b, 0x35, 0x53, (char)0xFF}},
    
    { .m = CTRL, .k = F5,          .seq = {0x1b, 0x5b, 0x31, 0x35, 0x3b, 0x35, 0x7e, (char)0xFF}},
    { .m = CTRL, .k = F6,          .seq = {0x1b, 0x5b, 0x31, 0x37, 0x3b, 0x35, 0x7e, (char)0xFF}},
    { .m = CTRL, .k = F7,          .seq = {0x1b, 0x5b, 0x31, 0x38, 0x3b, 0x35, 0x7e, (char)0xFF}},
    { .m = CTRL, .k = F8,          .seq = {0x1b, 0x5b, 0x31, 0x39, 0x3b, 0x35, 0x7e, (char)0xFF}},
    { .m = CTRL, .k = F9,          .seq = {0x1b, 0x5b, 0x32, 0x30, 0x3b, 0x35, 0x7e, (char)0xFF}},
    { .m = CTRL, .k = F10,         .seq = {0x1b, 0x5b, 0x32, 0x31, 0x3b, 0x35, 0x7e, (char)0xFF}},
    { .m = CTRL, .k = F11,         .seq = {0x1b, 0x5b, 0x32, 0x33, 0x3b, 0x35, 0x7e, (char)0xFF}},
    { .m = CTRL, .k = F12,         .seq = {0x1b, 0x5b, 0x32, 0x34, 0x3b, 0x35, 0x7e, (char)0xFF}},

    { .m = CTRL, .k = ARROW_UP,    .seq = {0x1b, 0x5b, 0x31, 0x3b, 0x35, 0x41, (char)0xFF}},
    { .m = CTRL, .k = ARROW_DOWN,  .seq = {0x1b, 0x5b, 0x31, 0x3b, 0x35, 0x42, (char)0xFF}},
    { .m = CTRL, .k = ARROW_RIGHT, .seq = {0x1b, 0x5b, 0x31, 0x3b, 0x35, 0x43, (char)0xFF}},
    { .m = CTRL, .k = ARROW_LEFT,  .seq = {0x1b, 0x5b, 0x31, 0x3b, 0x35, 0x44, (char)0xFF}},

    { .m = CTRL, .k = END,         .seq = {0x1b, 0x5b, 0x31, 0x3b, 0x35, 0x46, (char)0xFF}},
    { .m = CTRL, .k = HOME,        .seq = {0x1b, 0x5b, 0x31, 0x3b, 0x35, 0x48, (char)0xFF}},
    { .m = CTRL, .k = INSERT,      .seq = {0x1b, 0x5b, 0x32, 0x3b, 0x35, 0x7e, (char)0xFF}},
    { .m = CTRL, .k = DELETE,      .seq = {0x1b, 0x5b, 0x33, 0x3b, 0x35, 0x7e, (char)0xFF}},
    { .m = CTRL, .k = PAGEUP,      .seq = {0x1b, 0x5b, 0x35, 0x3b, 0x35, 0x7e, (char)0xFF}},
    { .m = CTRL, .k = PAGEDOWN,    .seq = {0x1b, 0x5b, 0x36, 0x3b, 0x35, 0x7e, (char)0xFF}},

    { .m = NONE, .k = F5,          .seq = {0x1b, 0x5b, 0x31, 0x35, 0x7e, (char)0xFF}},
    { .m = NONE, .k = F6,          .seq = {0x1b, 0x5b, 0x31, 0x37, 0x7e, (char)0xFF}},
    { .m = NONE, .k = F7,          .seq = {0x1b, 0x5b, 0x31, 0x38, 0x7e, (char)0xFF}},
    { .m = NONE, .k = F8,          .seq = {0x1b, 0x5b, 0x31, 0x39, 0x7e, (char)0xFF}},
    { .m = NONE, .k = F9,          .seq = {0x1b, 0x5b, 0x32, 0x30, 0x7e, (char)0xFF}},
    { .m = NONE, .k = F10,         .seq = {0x1b, 0x5b, 0x32, 0x31, 0x7e, (char)0xFF}},
    { .m = NONE, .k = F11,         .seq = {0x1b, 0x5b, 0x32, 0x33, 0x7e, (char)0xFF}},
    { .m = NONE, .k = F12,         .seq = {0x1b, 0x5b, 0x32, 0x34, 0x7e, (char)0xFF}},
    
    { .m = NONE, .k = INSERT,      .seq = {0x1b, 0x5b, 0x32, 0x7e, (char)0xFF}},
    { .m = NONE, .k = DELETE,      .seq = {0x1b, 0x5b, 0x33, 0x7e, (char)0xFF}},
    { .m = NONE, .k = PAGEUP,      .seq = {0x1b, 0x5b, 0x35, 0x7e, (char)0xFF}},
    { .m = NONE, .k = PAGEDOWN,    .seq = {0x1b, 0x5b, 0x36, 0x7e, (char)0xFF}},

    { .m = NONE, .k = ARROW_UP,    .seq = {0x1b, 0x5b, 0x41, (char)0xFF}},
    { .m = NONE, .k = ARROW_DOWN,  .seq = {0x1b, 0x5b, 0x42, (char)0xFF}},
    { .m = NONE, .k = ARROW_RIGHT, .seq = {0x1b, 0x5b, 0x43, (char)0xFF}},
    { .m = NONE, .k = ARROW_LEFT,  .seq = {0x1b, 0x5b, 0x44, (char)0xFF}},
    { .m = NONE, .k = END,         .seq = {0x1b, 0x5b, 0x46, (char)0xFF}},
    { .m = NONE, .k = HOME,        .seq = {0x1b, 0x5b, 0x48, (char)0xFF}},

    { .m = NONE, .k = F1,          .seq = {0x1b, 0x4f, 0x50, (char)0xFF}},
    { .m = NONE, .k = F2,          .seq = {0x1b, 0x4f, 0x51, (char)0xFF}},
    { .m = NONE, .k = F3,          .seq = {0x1b, 0x4f, 0x52, (char)0xFF}},
    { .m = NONE, .k = F4,          .seq = {0x1b, 0x4f, 0x53, (char)0xFF}},

    { .m = NONE, .k = ENTER,       .seq = {0x0d, (char)0xFF}},
    { .m = NONE, .k = ESCAPE,      .seq = {0x1b, (char)0xFF}},
    { .m = NONE, .k = TAB,         .seq = {0x09, (char)0xFF}},
    { .m = NONE, .k = BACKSPACE,   .seq = {0x7F, (char)0xFF}},

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

static int asSpecial(Key& k, KeyType t, Modifier m, int ret) {
    k.type = t;
    k.value = '~';
    k.modifier = m;
    return ret;
}

static int decodeCharSequence(AChar* buffer, AChar* limit, Key& k) {   
    int bufferSize = limit-buffer;
    for (KeySequence* entry = sequences; entry->k != ERROR; ++entry) {
        int seqLength = -1;
        while (entry->seq[++seqLength] != (char)0xFF);

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

#ifdef BUILDONWINDOWS

static KeyType checkType(KEY_EVENT_RECORD* of) {
    if (of->uChar.AsciiChar >= STANDARD_MODIFIED_START && of->uChar.AsciiChar < STANDARD_MODIFIED_END && of->uChar.AsciiChar != HEX_ENTER && of->uChar.AsciiChar != HEX_LF && of->uChar.AsciiChar != HEX_TAB
            && of->wVirtualKeyCode >= 0x30 && of->wVirtualKeyCode <= 0x5A // POOL of keyboard keys from 0 to Z
        ) {
        return STANDARD_MODIFIED;
    }
    
    else if (of->uChar.AsciiChar >= STANDARD_BEGIN && of->uChar.AsciiChar <= STANDARD_END) {
        if (of->dwControlKeyState & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED)) {
            return STANDARD_MODIFIED;
        }
        else {
            return STANDARD;
        }
    }
    else {
        return SPECIALS;
    }
}

static bool shouldDiscardEvent(INPUT_RECORD* ptr) {
    if (ptr->EventType != KEY_EVENT) return true;
    KEY_EVENT_RECORD& key = ptr->Event.KeyEvent;
    
    if (false == key.bKeyDown) return true;

    return false;
}

void InputDecoder::feed(INPUT_RECORD* ptr, int bufferSize) {
    Key k = {};
    for (int i = 0; i < bufferSize; i++) {
        INPUT_RECORD* here = ptr + i;
        if (shouldDiscardEvent(here)) {
            continue;
        }
        
        KEY_EVENT_RECORD* key = &here->Event.KeyEvent;
        KeyType type = checkType(key);

        if (type == STANDARD) {
            k.value = key->uChar.AsciiChar;
            k.type = STANDARD;
            k.modifier = NONE;
            queue.push(k);
        }
        else if (type == STANDARD_MODIFIED) {
            k.value = key->uChar.AsciiChar;
            k.type = STANDARD_MODIFIED;
            
            if (k.value >= STANDARD_MODIFIED_START && k.value < STANDARD_MODIFIED_END) {
                if (k.value == 0) {
                    k.value = ' ';
                }
                else {
                    k.value = k.value + STANDARD_MODIFIED_OFFSET;
                }
                k.type = STANDARD_MODIFIED;
                k.modifier = CTRL;
            }
            else if (key->dwControlKeyState & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED)) {
                k.modifier = META;
            }
            else {
                k.modifier = NONE;
            }

            queue.push(k);
        }
        else if (type == SPECIALS) {
            k.value = '?';
            if (key->dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) {
                k.modifier = CTRL;
            }
            else if (key->dwControlKeyState & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED)) {
                k.modifier = META;
            }
            else {
                k.modifier = NONE;
            }

            if (false);
            else if (key->wVirtualKeyCode == VK_RETURN) k.type = ENTER;
            else if (key->wVirtualKeyCode == VK_TAB) k.type = TAB;
            else if (key->wVirtualKeyCode == VK_ESCAPE) k.type = ESCAPE;
            else if (key->wVirtualKeyCode == VK_BACK) k.type = BACKSPACE;
                
            else if (key->wVirtualKeyCode == VK_UP) k.type = ARROW_UP;
            else if (key->wVirtualKeyCode == VK_DOWN) k.type = ARROW_DOWN;
            else if (key->wVirtualKeyCode == VK_RIGHT) k.type = ARROW_RIGHT;
            else if (key->wVirtualKeyCode == VK_LEFT) k.type = ARROW_LEFT;
                
            else if (key->wVirtualKeyCode == VK_END) k.type = END;
            else if (key->wVirtualKeyCode == VK_HOME) k.type = HOME;
                
            else if (key->wVirtualKeyCode == VK_F1) k.type = F1;
            else if (key->wVirtualKeyCode == VK_F2) k.type = F2;
            else if (key->wVirtualKeyCode == VK_F3) k.type = F3;
            else if (key->wVirtualKeyCode == VK_F4) k.type = F4;
                
            else if (key->wVirtualKeyCode == VK_F5) k.type = F5;
            else if (key->wVirtualKeyCode == VK_F6) k.type = F6;
            else if (key->wVirtualKeyCode == VK_F7) k.type = F7;
            else if (key->wVirtualKeyCode == VK_F8) k.type = F8;
            else if (key->wVirtualKeyCode == VK_F9) k.type = F9;
            else if (key->wVirtualKeyCode == VK_F10) k.type = F10;
            else if (key->wVirtualKeyCode == VK_F11) k.type = F11;
            else if (key->wVirtualKeyCode == VK_F12) k.type = F12;

            else if (key->wVirtualKeyCode == VK_INSERT) k.type = INSERT;
            else if (key->wVirtualKeyCode == VK_DELETE) k.type = DELETE;
            else if (key->wVirtualKeyCode == VK_PRIOR) k.type = PAGEUP;
            else if (key->wVirtualKeyCode == VK_NEXT) k.type = PAGEDOWN;

            else {
                k.type = ERROR;
            }

            if (k.type != ERROR) {
                queue.push(k);
            }
        }
    }
}

#else

static KeyType checkType(AChar* of) {
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
#endif

Key InputDecoder::load() {
    if (!canLoad()) {
        return SPECIAL_EMPTY;
    }
    Key ret = queue.front();
    queue.pop();
    return ret;
}