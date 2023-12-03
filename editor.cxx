#include "editor.h"

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

void Editor::consume(Key k) {
    text += (char)k.value;
    cursor = text.length();
}