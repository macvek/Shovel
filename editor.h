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

#include "inputdecoder.h"

class Editor {
    std::string text;
    std::string tabString;
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
    void putString(std::string s);
    void moveToLineStart();
    void moveToLineEnd();
    void moveLineUp();
    void moveLineDown();

    int boundCursor(int expectedPosition) const;

    void setTabString(std::string aTabString);
};
