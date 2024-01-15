#pragma once

#include <queue>
#include <string>

#ifdef BUILDONWINDOWS
#include "buildonwindows.h"
#endif
typedef char AChar;

enum KeyType {
    ERROR,
    EMPTY,
    STANDARD,
    SPECIALS, // MARKER for checking for special CHARACTERS by k.type > SPECIALS
    
    ENTER,
    TAB,
    ESCAPE,
    BACKSPACE,

    ARROW_UP,
    ARROW_DOWN,
    ARROW_RIGHT,
    ARROW_LEFT,
    
    END,
    HOME,

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

    INSERT,
    DELETE,
    PAGEUP,
    PAGEDOWN,

    STANDARD_MODIFIED   // marker for STANDARD key, but with applied modifier    
};

enum Modifier {
    NONE,
    CTRL,
    META
};

struct Key {
    AChar value;
    KeyType type;
    Modifier modifier;

    std::string typeName();
    std::string modifierName();
};

class InputDecoder {
    std::queue<Key> queue;

    public:
#ifdef BUILDONWINDOWS
    void feed(INPUT_RECORD* buffer, int size);
#else 
    void feed(AChar* buffer, int size);
#endif
    bool canLoad();
    Key load();
};