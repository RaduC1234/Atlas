#pragma once
#include "Atlas.hpp"


// Event callback function type
using EventCallbackFn = std::function<void(Event&)>;

// Event listener class
class EventListener {
public:
    void subscribe(EventType type, const EventCallbackFn& callback) {
        m_Callbacks[type].push_back(callback);
    }

    void unsubscribe(EventType type) {
        m_Callbacks.erase(type);
    }

    void dispatchEvent(Event& event) {
        auto type = event.getType();
        if (m_Callbacks.find(type) != m_Callbacks.end()) {
            for (auto& callback : m_Callbacks[type]) {
                callback(event);
                if (event.handled)
                    break;
            }
        }
    }

private:
    std::unordered_map<EventType, std::vector<EventCallbackFn>> m_Callbacks;
};

