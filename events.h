#pragma once

#include <string>

template<typename T> struct EventTrigger {
    virtual void trigger(std::string eventName, T* handler) = 0;
};

template<typename T> class Events {
    T* handler;

    public:
    void registerEvent(std::string eventName, T* handler);
    void triggerEvent(std::string eventName, EventTrigger<T> &trigger);
};

template<typename T> void Events<T>::registerEvent(std::string eventName, T* aHandler) {
    this->handler = aHandler;
}

template<typename T> void Events<T>::triggerEvent(std::string eventName, EventTrigger<T> &callToTrigger) {
    callToTrigger.trigger(eventName, this->handler);
}