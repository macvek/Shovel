#pragma once

#include "inputdecoder.h"

class Editor {
    std::string text;
    int cursor;

    public:
    Editor();
    Editor(std::string text);
    
    std::string getText();
    void setText(std::string);
    
    void consume(Key k);
    int getCursor();

    void moveCursor(int offset);
    void deleteAtCursor();
};
