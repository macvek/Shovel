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