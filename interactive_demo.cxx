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

void drawMenu() {
    int posX = 30;
    int posY = 1;
    int width = 20;
    int height = 10;

    int scrollY = 3;
    f.drawFrame(frontBuffer,posX,posY,posX+width+1,posY+height+1, Frame::SingleBorder);
    
    RenderBuffer menuBuffer(width,15,' ',true);
    for (int i=0;i<15;i++) {
        std::stringstream caption;
        caption << "Menu item #" << i+1;
        menuBuffer.writeText(caption.str(), 0, i);
        if ( i == selectedItem ) {
            menuBuffer.writeColorLine(0, i, 20, Terminal::MakeColor(Terminal::BLACK, Terminal::WHITE));
        }
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
