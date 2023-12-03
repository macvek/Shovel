
#include <iostream>
#include "editor.h"

using namespace std;

#define ERR cerr << __FUNCTION__ << " " 

void feedEditor(std::string input, Editor &e) {
    InputDecoder d;
    d.feed( (AChar*)input.c_str(),  input.length());
    while(d.canLoad()) {
        e.consume(d.load());
    }
}

void TestClearEditor() {
    Editor e;
    if ("" != e.getText()) {
        ERR << "Editor should return empty text" << endl;
        exit(1);
    }

    if (0 != e.getCursor()) {
        ERR << "Editor should place cursor after the text" << endl;
        exit(1);
    }

    e.setText("HelloWorld");
    if ("HelloWorld" != e.getText()) {
        ERR << "Editor should return HelloWorld" << endl;
        exit(1);
    }

    if (10 != e.getCursor()) {
        ERR << "Editor should place cursor after the text after setting" << endl;
        exit(1);
    }
}

void TestConsumeSampleInput() {
    Editor e;
    feedEditor("HelloWorld", e);
    
    if ("HelloWorld" != e.getText()) {
        ERR << "Editor should return HelloWorld" << endl;
        exit(1);
    }
}

void TestMovingCursorHomeEnd() {
    Editor e;
    e.setText("HelloWorld");
    e.moveCursor(-3);

    Key k;
    k.type = HOME;

    e.consume(k);

    if (e.getCursor() != 0) {
        ERR << "Cursor should be on pos 0, got : " << e.getCursor();
        exit(1);
    }

    k.type = END;
    e.consume(k);

    if (e.getCursor() != 10) {
        ERR << "Cursor should be on pos 10, got : " << e.getCursor();
        exit(1);
    }
}

void TestMovingCursorSingleLine() {
    Editor e;
    e.setText("HelloWorld");
    
    int cursor = e.getCursor();

    Key k;
    k.type = ARROW_LEFT;

    e.consume(k);
    
    if (cursor-1 != e.getCursor()) {
        ERR << "Left arrow should lower cursor" << endl;
        exit(1);
    }

    k.type = ARROW_RIGHT;
    e.consume(k);

    if (cursor != e.getCursor()) {
        ERR << "Right arrow should restore cursor to initial pos" << endl;
        exit(1);
    }

    e.consume(k);
    if (cursor != e.getCursor()) {
        ERR << "Right arrow again should not affect cursor and keep it in the end" << endl;
        exit(1);
    }

    k.type = ARROW_LEFT;
    for (int i=0;i<15;i++) e.consume(k);

    if (0 != e.getCursor()) {
        ERR << "Left arrow should eventually move cursor to beginning; got :" << e.getCursor() << endl;
        exit(1);
    }
}

int main() {
    TestClearEditor();
    TestConsumeSampleInput();
    TestMovingCursorSingleLine();
    TestMovingCursorHomeEnd();
}
