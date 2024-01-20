#pragma once
#include "renderbuffer.h"

class Frame {
    public:
    static const std::string Hash;
    static const std::string DashAndSlash;
    static const std::string SingleBorder;
    static const std::string DoubleBorder;
    

    void drawFrame(RenderBuffer& where, int left, int top, int right, int bottom, std::string input);
};

