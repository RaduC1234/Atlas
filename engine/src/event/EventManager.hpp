#pragma once

#include "core/Core.hpp"

class EventManager {
public:

    static void init() {
        if (!s_Listener)
            s_Listener = std::make_unique<EventListener>();
    }

    static void subscribe(EventType type, const EventCallbackFn& callback) {
        if (s_Listener) {
            s_Listener->subscribe(type, callback);
        }
    }

    static void unsubscribe(EventType type) {
        if (s_Listener) {
            s_Listener->unsubscribe(type);
        }
    }

    static void dispatch(Event& event) {
        if (s_Listener) {
            s_Listener->dispatchEvent(event);
        }
    }


    static EventDispatcher createDispatcher(Event& event) {
        return EventDispatcher(event);
    }

private:
    static std::unique_ptr<EventListener> s_Listener;
};

std::unique_ptr<EventListener> EventManager::s_Listener = nullptr;


/**
#include "EventManager.hpp"
#include <iostream>

// Custom event class inheriting from Event
class KeyPressedEvent : public Event {
public:
    KeyPressedEvent(int keyCode) : keyCode(keyCode) {}

    EventType getType() const override {
        return EventType::KEY_PRESSED;
    }

    static EventType getStaticType() {
        return EventType::KEY_PRESSED;
    }

    int keyCode;
};

int main() {
    // Initialize EventManager
    EventManager::init();

    // Subscribe to a key press event
    EventManager::subscribe(EventType::KEY_PRESSED, [](Event& event) {
        auto& keyEvent = static_cast<KeyPressedEvent&>(event);
        std::cout << "Key Pressed: " << keyEvent.keyCode << std::endl;
        event.handled = true;
    });

    // Create and dispatch a key pressed event
    KeyPressedEvent keyEvent(65); // Example key code
    EventManager::dispatch(keyEvent);

    return 0;
}

*/
