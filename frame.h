#pragma once
#include "renderbuffer.h"

class Frame {
    public:
    static const std::string HashFrame;

    void drawFrame(RenderBuffer& where, int left, int top, int right, int bottom, std::string input);
};

