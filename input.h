#pragma once

#include <queue>
#include "inputdecoder.h"

class Input {
    AChar buffer[16];
    std::queue<Key> keyPressed;
    InputDecoder decoder;

    void decodeBuffer(int);

    public:
    void waitFor();
    Key getKey();
};