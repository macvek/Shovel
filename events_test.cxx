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

struct TriggerMyEvent : public EventTrigger<Handler> {
    void trigger(const std::string& eventName, Handler* handler) {
        if ("MyEvent" == eventName) {
            handler->onEvent();
        }
        if ("ConsumeInt" == eventName) {
            handler->onConsumeInt(123);
        }
    }
};

int main() {
    Events<Handler> e;
    MyHandler h; 
    TriggerMyEvent triggerMyEvent;
    
    e.registerEvent("MyEvent", &h);
    e.registerEvent("MyEvent", &h);
    e.registerEvent("ConsumeInt", &h);
    e.triggerEvent("MyEvent", triggerMyEvent);
    e.triggerEvent("ConsumeInt", triggerMyEvent);
    e.triggerEvent("UnknownEvent", triggerMyEvent);

    return 0;
}