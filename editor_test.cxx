#include <iostream>
#include "editor.h"

#ifdef BUILDONWINDOWS
#include "buildonwindows.h"
#endif

using namespace std;

#define ERR cerr << __FUNCTION__ << " " 

void feedEditor(std::string input, Editor &e) {
    InputDecoder d;
#ifdef BUILDONWINDOWS
    INPUT_RECORD rec = {};
    rec.EventType = KEY_EVENT;
    rec.Event.KeyEvent.bKeyDown = 1;
    for (int i = 0; i < input.length(); ++i) {
        rec.Event.KeyEvent.uChar.AsciiChar = input[i];
        d.feed(&rec, 1);
    }
#else 
    d.feed((AChar*)input.c_str(), input.length());
#endif
    
    while(d.canLoad()) {
        e.consume(d.load());
    }
}

void assertCursor(Editor &e, int expectedPosition) {
    if (expectedPosition != e.getCursor()) {
        ERR << "Cursor should be at " << expectedPosition << ", got " << e.getCursor() << endl;
        exit(1);
    }
}

void assertStat(Editor &e, int column, int line) {
    if (column != e.getCurrentColumn()) {
        ERR << "Column should be at " << column << ", got " << e.getCurrentColumn() << endl;
        exit(1);
    }

    if (line != e.getCurrentLine()) {
        ERR << "Line should be at " << line << ", got " << e.getCurrentLine() << endl;
        exit(1);
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
    
    auto text = e.getText();
    if ("HelloWorld" != text) {
        ERR << "Editor should return HelloWorld, got " << text << endl;
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

void TestInsertingText() {
    Editor e;
    e.setText("Hello");
    feedEditor("World", e);

    if ("HelloWorld" != e.getText()) {
        ERR << "Should be HelloWorld, got "<< e.getText() << endl;
        exit(1);
    }

    Key k;
    k.type = ARROW_LEFT;
    for (int i=0;i<5;i++) e.consume(k);

    feedEditor("Magic", e);

    if ("HelloMagicWorld" != e.getText()) {
        ERR << "Should be HelloMagicWorld, got "<< e.getText() << endl;
        exit(1);
    }


    k.type = HOME;
    e.consume(k);
    feedEditor("Prefix", e);
    
    k.type = END;
    e.consume(k);
    feedEditor("Suffix", e);

    if ("PrefixHelloMagicWorldSuffix" != e.getText()) {
        ERR << "Should be PrefixHelloMagicWorldSuffix, got "<< e.getText() << endl;
        exit(1);
    }
}

void TestDeletingText() {
    Editor e;
    e.setText("HelloWorld");
    
    Key k;
    k.type = ARROW_LEFT;
    for (int i=0;i<3;i++) e.consume(k);

    k.type = DELETE;
    for (int i=0;i<3;i++) e.consume(k);

    k.type = BACKSPACE;
    for (int i=0;i<3;i++) e.consume(k);

    if ("Hell" != e.getText()) {
        ERR << "Should be Hell, got " << e.getText() << endl;
        exit(1);
    }
}

void TestAddingLineEnding() {
    Editor e;
    Key k;

    feedEditor("HelloWorld", e);
    k.type = ENTER;
    e.consume(k);

    if ("HelloWorld\n" != e.getText()) {
        ERR << "Should be be Helloworld and line ending, got " << e.getText() << endl;
        exit(1);
    }
}

void TestMovingCursorHomeInLine() {
    Editor e;
    Key k;

    e.setText("\nHelloWorld");
    k.type = HOME;
    e.consume(k);

    assertCursor(e, 1);
}

void TestMovingCursorEndInLine() {
    Editor e;
    Key k;

    e.setText("END\n");
    e.setCursor(0);

    k.type = END;
    e.consume(k);

    assertCursor(e, 3);
}

void TestMovingCursorArrowDown() {
    Editor e;
    Key k;

    e.setText(
        "Idx0\n"
        "Idx5 - this line is longer then next one\n"
        "idx46\n"
        "Idx52, here line does not end with LF"
    );
    e.setCursor(0);

    k.type = ARROW_DOWN;
    e.consume(k);

    assertCursor(e, 5);

    e.consume(k);
    assertCursor(e, 46);

    e.consume(k);
    assertCursor(e, 52);

    e.consume(k);
    assertCursor(e, e.getText().length());
}

void TestMovingCursorArrowUp() {
    Editor e;
    Key k;

    e.setText(
        "Idx0\n"
        "Idx5\n"
        "Idx10, here line does not end with LF"
    );

    k.type = ARROW_UP;
    e.consume(k);

    assertCursor(e, 9);

    e.consume(k);
    assertCursor(e, 4);

    e.consume(k);
    assertCursor(e, 0);
    
}

void TestMovingCursorArrowUpAndDownFromEndOfString() {
    Editor e;
    Key k;

    e.setText(
        "THIS LINE HAS 40 CHARACTERS............\n"
        "THIS LINE HAS 30 CHARACTERS..\n"
        "THIS LINE HAS 40 CHARACTERS............\n"
    );

    e.setCursor(0);
    k.type = END;
    e.consume(k);

    assertCursor(e, 39);
    k.type = ARROW_DOWN;
    e.consume(k);
    assertCursor(e, 69);

    e.consume(k);
    assertCursor(e, 109);

    e.consume(k);
    assertCursor(e, 110);

    k.type = ARROW_UP;
    e.consume(k);
    assertCursor(e, 109);

    e.consume(k);
    assertCursor(e, 69);

    e.consume(k);
    assertCursor(e, 39);
}

void TestStats() {
    Editor e;
    Key k;

    e.setText(
        "THIS LINE HAS 40 CHARACTERS............\n"
        "THIS LINE HAS 30 CHARACTERS..\n"
        "THIS LINE HAS 40 CHARACTERS............\n"
    );

    e.setCursor(0);
    assertStat(e, 0,0);

    e.setCursor(45);
    assertStat(e, 5,1);
    
    k.type = ARROW_UP; e.consume(k);
    assertStat(e, 5,0);

    k.type = ARROW_RIGHT; e.consume(k);
    assertStat(e, 6, 0);

    k.type = ARROW_DOWN; e.consume(k);
    assertStat(e, 6, 1);

    k.type = ARROW_LEFT; e.consume(k);
    assertStat(e, 5, 1);

    k.type = ARROW_UP; e.consume(k);
    assertStat(e, 5, 0);
}


void TestEditorConsumesErrorAndIgnoresIt() {
    Editor e;
    if ("" != e.getText()) {
        ERR << "Editor should return empty text" << endl;
        exit(1);
    }

    Key k;
    k.type = ERROR;
    k.value = '@';
    e.consume(k);

    if ("" != e.getText()) {
        ERR << "Editor should return empty text after consuming ERROR key, got " << e.getText() << endl;
        exit(1);
    }

}


int main() {
    TestClearEditor();
    TestConsumeSampleInput();
    TestMovingCursorSingleLine();
    TestMovingCursorHomeEnd();
    TestInsertingText();
    TestDeletingText();
    TestAddingLineEnding();
    TestMovingCursorHomeInLine();
    TestMovingCursorEndInLine();
    TestMovingCursorArrowDown();
    TestMovingCursorArrowUp();
    TestMovingCursorArrowUpAndDownFromEndOfString();
    TestStats();
    TestEditorConsumesErrorAndIgnoresIt();
}
