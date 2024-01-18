#pragma once
#include "renderbuffer.h"

class Frame {
    public:
    void drawFrame(RenderBuffer& where, int left, int top, int right, int bottom);
};

