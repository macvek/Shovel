#pragma once

#include "inputdecoder.h"

class Editor {
    std::string text;
    int cursor;
    int lineOffset;

    public:
    Editor();
    Editor(std::string text);
    
    std::string getText();
    void setText(std::string);
    
    void consume(Key k);
    int getCursor();
    void setCursor(int where);

    void moveCursor(int offset);
    void deleteAtCursor();

    void putChar(AChar c);
    void moveToLineStart();
    void moveToLineEnd();
};
