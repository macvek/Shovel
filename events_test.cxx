#include "events.h"
#include <iostream>

using namespace std;

struct Handler {
    virtual void onEvent() {}
    virtual void onConsumeInt(int value) {}
};

struct MyHandler : public Handler {
    void onEvent() {
        cout << "Got Event!" << endl;
    }

    void onConsumeInt(int it) {
        cout << "On consume event " << it <<  endl;
    }
};

static int counter = 0;

struct TriggerMyEvent : public EventTrigger<Handler, void> {
    void trigger(const std::string& eventName, Handler* handler, void*) {
        if ("MyEvent" == eventName) {
            handler->onEvent();
        }
        if ("ConsumeInt" == eventName) {
            handler->onConsumeInt(++counter);
        }
    }
};

int main() {
    Events<Handler, void> e;
    MyHandler h; 
    TriggerMyEvent triggerMyEvent;
    
    e.registerEvent("MyEvent", &h);
    e.registerEvent("MyEvent", &h);
    e.registerEvent("ConsumeInt", &h);

    e.triggerEvent("MyEvent", triggerMyEvent, nullptr);
    e.triggerEvent("ConsumeInt", triggerMyEvent, nullptr);
    e.triggerEvent("UnknownEvent", triggerMyEvent, nullptr);

    if (counter != 1) {
        cerr << "Failed, counter should be 1";
        exit(1);
    }
    e.unregisterEvent("ConsumeInt", &h);
    e.triggerEvent("ConsumeInt", triggerMyEvent, nullptr);

    if (counter != 1) {
        cerr << "Failed, counter should be 1, as event was unregistered";
        exit(1);
    }

    e.registerEvent("ConsumeInt", &h);
    e.triggerEvent("ConsumeInt", triggerMyEvent, nullptr);

    if (counter != 2) {
        cerr << "Failed, counter should be 2, got: " << counter<< ". Event was registered again and retriggered";
        exit(1);
    }

    return 0;
}