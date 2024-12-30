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

#include "editor.h"
#include <string>
Editor::Editor() : Editor(""){}

Editor::Editor(std::string aText) : tabString("    "), text(aText) {
    cursor = text.length();
    resetExpectedOffset();
}

std::string Editor::getText() {
   return text;
}

int Editor::getCursor() const {
    return cursor;
}

void Editor::setCursor(int where) {
    if (where < 0) { 
        cursor = 0;
    }
    else if (where > text.length()) {
        cursor = text.length();
    }
    else {
        cursor = where;
    }
}

void Editor::setText(std::string aText) {
    text = aText;
    cursor = text.length();
}

void Editor::deleteAtCursor() {
    if (cursor < text.length()) {
        text.erase(text.cbegin()+cursor);
    }
}

void Editor::consume(Key k) {
    if (k.type >= SPECIALS) {
        if (k.type == TAB) {
            putString(tabString);
        } 
        else if (k.type == ENTER) {
            putChar('\n');
        }
        else if (k.type == DELETE) {
            deleteAtCursor();
        }
        else if (k.type == BACKSPACE) {
            if (cursor > 0) {
                moveCursor(-1);
                deleteAtCursor();
            }
        }
        else if (k.type == HOME) {
            moveToLineStart();
        }
        else if (k.type == END) {
            moveToLineEnd();
        } 
        else if (k.type == ARROW_RIGHT) {
            moveCursor(1);
        }
        else if (k.type == ARROW_LEFT) {
            moveCursor(-1);
        }
        else if (k.type == ARROW_DOWN) {
            moveLineDown();
        }
        else if (k.type == ARROW_UP) {
            moveLineUp();
        }
    } else if (k.type != ERROR) {
        putChar(k.value);
    }
}

int Editor::boundCursor(int expectedPosition) const {
    return expectedPosition < 0 ? 0 : ( expectedPosition > text.length() ? text.length() : expectedPosition);
}

int Editor::findLineStart() const {
    return findLineStart(cursor);
}

int Editor::findLineStart(int from) const {
    auto here = text.begin() + boundCursor(from);
    for (;;) {
        if (here > text.begin()) {
            --here;
        }

        if ( *here == '\n') {
            return (here - text.begin()) + 1;
        }

        if (here == text.begin()) {
            break;
        }
    }

    return 0;
}

int Editor::findLineEnd() const{
    return findLineEnd(cursor);
}

int Editor::findLineEnd(int from) const {
    for (auto here = text.begin() + boundCursor(from); here < text.end(); ++here) { 
        if ( *here == '\n') {
            return here - text.begin();
        }
    }

    return text.length();
}

void Editor::moveToLineStart() {
    resetExpectedOffset();
    cursor = findLineStart();
}

void Editor::moveToLineEnd() {
    resetExpectedOffset();
    cursor = findLineEnd();
}

int Editor::getCurrentColumn() const {
    return cursor - findLineStart();
}

int Editor::getCurrentLine() const {
    int count = 0;
    int pointer = cursor;
    for (;;) {
        int lineStart = findLineStart(pointer);
        if (lineStart == 0) {
            return count;
        }
        else {
            pointer = lineStart - 1;
            ++count;
        }
    }

}

void Editor::moveLineUp() {
    updateExpectedOffset();
    int currentLineStart = findLineStart();
    if (currentLineStart == 0) {
        // up arrow on beginning of a file => reset expectedOffset
        if (cursor == 0) {
            resetExpectedOffset();
        }
        else {
            cursor = 0;
            updateExpectedOffset();
        }
        return;
    }

    int prevLineEnd = currentLineStart - 1;
    int prevLineStart = findLineStart(prevLineEnd);

    adjustExpectedOffset(prevLineStart, prevLineEnd);
}

void Editor::moveLineDown() {
    updateExpectedOffset();
    int currentLineEnd = findLineEnd();

    if (currentLineEnd == text.length()) {
        // down arrow on end of a file => reset expectedOffset
        if (cursor == currentLineEnd) {
            resetExpectedOffset();
        }
        else {
            cursor = currentLineEnd;
            updateExpectedOffset();
        }
        
        return;
    }
    
    int nextLineStart = currentLineEnd + 1;
    int nextLineEnd = findLineEnd(nextLineStart);

    adjustExpectedOffset(nextLineStart, nextLineEnd);
}

void Editor::adjustExpectedOffset(int lineStart, int lineEnd) {
    int expectedPos = lineStart + loadExpectedOffset();
    if (expectedPos > lineEnd) {
        cursor = lineEnd;
    }
    else {
        cursor = expectedPos;
    }

    updateExpectedOffset();
}

void Editor::putChar(AChar c) {
    text.insert(text.cbegin() + cursor , c);
    ++cursor;
    resetExpectedOffset();
}

void Editor::putString(std::string s) {
    text.insert(cursor, s);
    cursor += s.length();
    resetExpectedOffset();
}

void Editor::moveCursor(int offset) {
    resetExpectedOffset();
    int nCursor = cursor + offset;
    int signedLength = text.length(); 

    if (nCursor > signedLength) {
        nCursor = signedLength;
    }
    else if (nCursor < 0) {
        nCursor = 0;
    }
    cursor = nCursor;

}

void Editor::updateExpectedOffset() {
    int currentOffset = getCurrentColumn();
    if (storedOffset == -1) {
        storedOffset = currentOffset;
    }
    else if (currentOffset == storedOffset) {
        storedOffset = -1;
    }
}

int Editor::loadExpectedOffset() {
    return storedOffset;
}

void Editor::resetExpectedOffset() {
    storedOffset = -1;
}

void Editor::setTabString(std::string aTabString) {
    tabString = aTabString;
}

