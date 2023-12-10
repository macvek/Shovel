#include "editor.h"
#include <string>
Editor::Editor() : Editor(""){}

Editor::Editor(std::string aText) : text(aText) {
    cursor = text.length();
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
    } else {
        putChar(k.value);
    }
}

int Editor::findLineStart() const {
    for (auto here = text.begin() + cursor; here-- > text.begin();) { 
        if ( *here == '\n') {
            return (here - text.begin()) + 1;
        }
    }

    return 0;
}

int Editor::findLineEnd() const{
    for (auto here = text.begin() + cursor; here < text.end(); ++here) { 
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
    return findLineStart() - cursor;
}

void Editor::moveLineDown() {
    int nextLineStart = findLineEnd() + 1;
    int offset = offsetInLine();

    int expectedPos = nextLineStart + offset;
    cursor = expectedPos > text.length() ? text.length() : expectedPos;
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