#pragma once

#include <queue>
#include "inputdecoder.h"

#ifdef BUILDONWINDOWS
#include "buildonwindows.h"
#endif

class Input {
#ifdef BUILDONWINDOWS
    INPUT_RECORD buffer[8];
#else
    AChar buffer[8];
#endif
    std::queue<Key> keyPressed;
    InputDecoder decoder;

    void decodeBuffer(int);

    public:
    void waitFor();
    Key getKey();
};