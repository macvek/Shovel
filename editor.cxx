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
            cursor = text.length();
        } 
        else if (k.type == ARROW_RIGHT) {
            moveCursor(1);
        }
        else if (k.type == ARROW_LEFT) {
            moveCursor(-1);
        }
    } else {
        putChar(k.value);
    }
}

void Editor::moveToLineStart() {
    for (auto here = text.begin() + cursor; here-- > text.begin();) { 
        if ( *here == '\n') {
            cursor = (here - text.begin()) + 1;
            return;
        }
    }

    cursor = 0;
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