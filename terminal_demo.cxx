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
