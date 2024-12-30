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

#include "console.h"
#include "terminal.h"
#include "input.h"
#include "events.h"
#include "renderbuffer.h"
#include "frame.h"
#include <cstdlib>
#include <sstream>

struct OnKeyHandler {
    virtual void onKey(Key &k) {}
};

Console console;
Terminal terminal(std::cout);
Input input;
Events<OnKeyHandler, Key> onKeyEvents;
bool stopApplication = false;

SpecialCharsMap base = SpecialCharsMapFactory::create();
RenderBuffer blankScene(80,24,' ',true, base);
RenderBuffer backBuffer = blankScene;
RenderBuffer frontBuffer = blankScene;
Frame f;

int selectedItem = 0;
int allItems = 15;
int scrollY = 0;
void drawMenu() {
    int posX = 30;
    int posY = 1;
    int width = 20;
    int height = 10;

    
    f.drawFrame(frontBuffer,posX,posY,posX+width+1,posY+height+1, Frame::SingleBorder);
    
    RenderBuffer menuBuffer(width,15,' ',true);
    for (int i=0;i<allItems;i++) {
        std::stringstream caption;
        caption << "Menu item #" << i+1;
        menuBuffer.writeText(caption.str(), 0, i);
        if ( i == selectedItem ) {
            menuBuffer.writeColorLine(0, i, 20, Terminal::MakeColor(Terminal::BLACK, Terminal::WHITE));
        }
    }

    int firstVisible = scrollY;
    if (selectedItem < firstVisible) {
        scrollY = selectedItem;
    }

    int lastVisible = scrollY + height-1 ;
    if (lastVisible < selectedItem) {
        scrollY = selectedItem - ( height -1 );
    }



    frontBuffer.writeView(menuBuffer.view(0,20,scrollY,10 + scrollY), posX+1, posY+1);
}

void render() {
    frontBuffer.copyFrom(blankScene);
    drawMenu();
    frontBuffer.toTerminal(terminal, 1, 1, true);
    terminal.flush();
}


struct KeyHandlerCapture : public OnKeyHandler {
    void onKey(Key &k) {
        if (k.type == KeyType::ESCAPE) {
            stopApplication = true;
            return;
        }
        
        if (k.type == KeyType::ARROW_DOWN) {
            ++selectedItem;
        }
        else if (k.type == KeyType::ARROW_UP) {
            --selectedItem;
        }
        
        selectedItem = selectedItem < 0 ? 0 : selectedItem >= allItems ? allItems-1 : selectedItem;
        render();
    }
} mainKeyCapture;


struct TriggerMyEvent : public EventTrigger<OnKeyHandler, Key> {
    Key* payload;
    void trigger(const std::string&, OnKeyHandler* handler, Key* k) {
        handler->onKey(*k);
    }
} triggerKeyEvent;

void mainLoop() {
    for (;;) {
        input.waitFor();
        Key k = input.getKey();

        onKeyEvents.triggerEvent("Key", triggerKeyEvent, &k);
        if (stopApplication) {
            break;
        }
    }
}

int main(int argc, char** argv) {
    console.enableRaw();
    terminal.showCursor(false);
    terminal.clear();
    terminal.flush();
    
    onKeyEvents.registerEvent("Key", &mainKeyCapture);

    mainLoop();

    console.restore();
    terminal.showCursor(true);
}
