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