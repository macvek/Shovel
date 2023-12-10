#pragma once

#include "inputdecoder.h"

class Editor {
    std::string text;
    int cursor;
    int storedOffset;
    void updateExpectedOffset();
    int loadExpectedOffset();
    void resetExpectedOffset();
    void adjustExpectedOffset(int lineStart, int lineEnd);

    public:
    Editor();
    Editor(std::string text);
    
    std::string getText();
    void setText(std::string);
    
    void consume(Key k);
    
    int findLineStart() const;
    int findLineStart(int from) const;
    int findLineEnd() const;
    int findLineEnd(int from) const;
    int getCurrentColumn() const;
    int getCurrentLine() const;
    
    int getCursor() const;
    void setCursor(int where);

    void moveCursor(int offset);
    void deleteAtCursor();

    void putChar(AChar c);
    void moveToLineStart();
    void moveToLineEnd();
    void moveLineUp();
    void moveLineDown();

    int boundCursor(int expectedPosition) const;
};
