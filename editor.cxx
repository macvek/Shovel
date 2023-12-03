#include "editor.h"
#include <string>
Editor::Editor() : Editor(""){}

Editor::Editor(std::string aText) : text(aText) {
    cursor = text.length();
}

std::string Editor::getText() {
   return text;
}

int Editor::getCursor() {
    return cursor;
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
        if (k.type == DELETE) {
            deleteAtCursor();
        }
        if (k.type == BACKSPACE) {
            if (cursor > 0) {
                moveCursor(-1);
                deleteAtCursor();
            }
        }
        if (k.type == HOME) {
            cursor = 0;
        }
        else if (k.type == END) {
            cursor = text.length();
        } 
        else if (k.type == ARROW_RIGHT) {
            moveCursor(1);
        }
        else if (k.type == ARROW_LEFT) {
            moveCursor(-1);
        }
    } else {
        text.insert(text.cbegin() + cursor , k.value);
        ++cursor;
    }
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