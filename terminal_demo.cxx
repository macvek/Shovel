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

#include "terminal.h"


using namespace std;

int main(int argc, char** argv) {
    Terminal t(cout);
    t.clear();
    t.foreColor(Terminal::RED);
    cout << "I'm red" << endl;

    t.foreColor(Terminal::BRIGHT_RED);
    t.backColor(Terminal::RED);
    cout << "I'm red with background" << endl;

    t.foreColorRGB(10,255,10);
    cout << "I'm using rgb" << endl;

    t.foreColorRGB(255,127,127);
    t.backColorRGB(127,0,0);
    cout << "I'm rgb with background" << endl;
    t.backDefault();
    cout << "Background to default" << endl;
    
    t.foreDefault();
    cout << "Foreground to default" << endl;

    t.foreColor(Terminal::GREEN);
    t.backColor(Terminal::YELLOW);
    cout << "Text before reset" << endl;

    t.reset();
    cout << endl << "after reset "<< endl;

    t.moveCursor(0,10);

    t.moveCursor(1,-5);
    cout << "A Moved 1, -5";
    t.moveCursor(0,1);
    cout << "B line below";
    t.moveCursor(0,-2);
    cout <<"C 2 lines above";
    
    cout <<"\n";
    t.moveCursor(30,0);
    cout << "D +30 x";
    t.moveCursor(-15,0);
    cout << "E -15 x";

    t.placeCursor(1,10);
    t.moveCursor(5,1);

    cout << "Calculated (X,Y) : ("<< t.getCalculatedX() << "," << t.getCalculatedY() << ")" << endl;

    t.placeCursor(50,20);
    cout << "Moved to 50,20" << endl;

}
