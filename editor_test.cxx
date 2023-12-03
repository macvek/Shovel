
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

int main() {
    TestClearEditor();
    TestConsumeSampleInput();
}
