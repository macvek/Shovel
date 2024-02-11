#include "events.h"
#include <iostream>

using namespace std;

struct Handler {
    virtual void onEvent() {}
};

struct MyHandler : public Handler {
    void onEvent() {
        cout << "Got Event!" << endl;
    }
};

struct TriggerMyEvent : public EventTrigger<Handler> {
    void trigger(const std::string& eventName, Handler* handler) {
        handler->onEvent();
    }
};

int main() {
    Events<Handler> e;
    MyHandler h; 
    TriggerMyEvent triggerMyEvent;
    
    e.registerEvent("MyEvent", &h);
    e.triggerEvent("MyEvent", triggerMyEvent);

    return 0;
}