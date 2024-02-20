#include "console.h"
#include "terminal.h"
#include "input.h"
#include "events.h"

struct OnKeyHandler {
    virtual void onKey(Key &k) {}
};

Console console;
Terminal terminal(std::cout);
Input input;
Events<OnKeyHandler, Key> onKeyEvents;
bool stopApplication = false;

struct KeyHandlerCapture : public OnKeyHandler {
    void onKey(Key &k) {
        if (k.type == KeyType::ESCAPE) {
            stopApplication = true;
        }
        else {
            std::cout << ".";
            std::cout.flush();
        }
    }
} mainKeyCapture;




struct TriggerMyEvent : public EventTrigger<OnKeyHandler, Key> {
    Key* payload;
    void trigger(const std::string&, OnKeyHandler* handler, Key* k) {
        handler->onKey(*k);
    }
} triggerKeyEvent;


int main(int argc, char** argv) {
    console.enableRaw();
    terminal.showCursor(false);
    terminal.clear();
    
    onKeyEvents.registerEvent("Key", &mainKeyCapture);
    for (;;) {
        input.waitFor();
        Key k = input.getKey();

        onKeyEvents.triggerEvent("Key", triggerKeyEvent, &k);
        if (stopApplication) {
            break;
        }
    }

    console.restore();
    terminal.showCursor(true);
}
