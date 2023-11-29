#pragma once

#include <vector>

struct KeyPressed {
    char c;
};

class Input {
    int key;
    char buffer[8];
    std::vector<KeyPressed> keyPressed;

    void decodeBuffer(int);

    public:
    static const int KEY_FAILED = -1;
    void waitFor();
    int getKey();
};