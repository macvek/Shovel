#pragma once

#include <string>
#include <unordered_set>
#include <unordered_map>

template<typename T> struct EventTrigger {
    virtual void trigger(const std::string& eventName, T* handler) = 0;
};

template<typename T> class Events {
    std::unordered_map<std::string, std::unordered_set<T*>> hub;

    public:
    void registerEvent(std::string eventName, T* handler);
    void unregisterEvent(std::string eventName, T* handler);
    void triggerEvent(std::string eventName, EventTrigger<T> &trigger);
};

template<typename T> void Events<T>::registerEvent(std::string eventName, T* aHandler) {
    this->hub[eventName].insert(aHandler);
}

template<typename T> void Events<T>::unregisterEvent(std::string eventName, T* aHandler) {
    if (this->hub.find(eventName) == this->hub.end()) {
        return;
    }
    
    this->hub[eventName].erase(aHandler);

    if (this->hub[eventName].empty()) {
        this->hub.erase(eventName);
    }
    
}


template<typename T> void Events<T>::triggerEvent(std::string eventName, EventTrigger<T> &callToTrigger) {
    if (this->hub.find(eventName) == this->hub.end()) {
        return;
    }

    auto matching = this->hub[eventName];
    for (auto each = matching.cbegin(); each != matching.end(); ++each) {
        callToTrigger.trigger(eventName, *each);
    }
}