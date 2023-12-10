#include "editor.h"
#include <string>
Editor::Editor() : Editor(""){}

Editor::Editor(std::string aText) : text(aText) {
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
        if (k.type == ENTER) {
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
    } else {
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
    for (auto here = text.begin() + boundCursor(from); here-- > text.begin();) { 
        if ( *here == '\n') {
            return (here - text.begin()) + 1;
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
    cursor = findLineStart();
}

void Editor::moveToLineEnd() {
    cursor = findLineEnd();
}

int Editor::offsetInLine() const {
    return cursor - findLineStart();
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
}

void Editor::moveCursor(int offset) {
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
    int currentOffset = offsetInLine();
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
