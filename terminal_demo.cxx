#include "terminal.h"


#define COLOR_RESET         0
#define COLOR_RED           1
#define COLOR_BRIGHT_RED    61


using namespace std;

int main(int argc, char** argv) {
    Terminal t(cout);
    t.foreColor(COLOR_RED);
    cout << "I'm red" << endl;

    t.foreColor(COLOR_BRIGHT_RED);
    t.backColor(COLOR_RED);
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

    t.foreColor(4);
    t.backColor(3);
    cout << "Text before reset" << endl;

    t.reset();
    cout << endl << "after reset "<< endl;

    cout << "extra 10 lines for some space...";
    cout << "\n\n\n\n\n \n\n\n\n\n";

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

    t.moveCursor(0,5);
    cout << endl;

}
