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

#pragma once

#include <string>
#include <unordered_set>
#include <unordered_map>

template<typename T, typename U> struct EventTrigger {
    virtual void trigger(const std::string& eventName, T* handler, U* payload) = 0;
};

template<typename T, typename U> class Events {
    std::unordered_map<std::string, std::unordered_set<T*>> hub;

    public:
    void registerEvent(std::string eventName, T* handler);
    void unregisterEvent(std::string eventName, T* handler);
    void triggerEvent(std::string eventName, EventTrigger<T,U> &trigger, U *payload);
};

template<typename T, typename U> void Events<T, U>::registerEvent(std::string eventName, T* aHandler) {
    this->hub[eventName].insert(aHandler);
}

template<typename T, typename U> void Events<T, U>::unregisterEvent(std::string eventName, T* aHandler) {
    if (this->hub.find(eventName) == this->hub.end()) {
        return;
    }
    
    this->hub[eventName].erase(aHandler);

    if (this->hub[eventName].empty()) {
        this->hub.erase(eventName);
    }
    
}

template<typename T, typename U> void Events<T,U>::triggerEvent(std::string eventName, EventTrigger<T,U> &callToTrigger, U *payload) {
    if (this->hub.find(eventName) == this->hub.end()) {
        return;
    }

    auto matching = this->hub[eventName];
    for (auto each = matching.cbegin(); each != matching.end(); ++each) {
        callToTrigger.trigger(eventName, *each, payload);
    }
}