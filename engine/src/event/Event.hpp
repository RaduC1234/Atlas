#pragma once
#include "Atlas.hpp"

// Event types enum
enum class EventType {
    NONE = 0,
    KEY_PRESSED, KEY_RELEASED,
    MOUSE_BUTTON_PRESSED, MOUSE_BUTTON_RELEASED, MOUSE_MOVED, MOUSE_SCROLLED
};

// Base event class
class Event {
public:
    virtual ~Event() = default;
    virtual EventType getType() const = 0;
    bool handled = false;
};

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

// Event dispatcher helper class
class EventDispatcher {
public:
    EventDispatcher(Event& event)
        : m_Event(event) {}

    template<typename T, typename F>
    bool dispatch(const F& func) {
        if (m_Event.getType() == T::getStaticType()) {
            m_Event.handled |= func(static_cast<T&>(m_Event));
            return true;
        }
        return false;
    }

private:
    Event& m_Event;
};