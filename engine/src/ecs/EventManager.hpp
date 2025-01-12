#pragma once

#include <iostream>
#include <functional>
#include <unordered_map>
#include <typeindex>
#include <vector>
#include <memory>
#include <stdexcept>

class Event {
public:
    virtual ~Event() = default;
};

class EventManager {
public:
    // Emit an event of type `EventType` to all subscribed listeners
    template<typename EventType>
    void emit(const EventType& event) const {
        auto it = listeners.find(typeid(EventType));
        if (it != listeners.end()) {
            for (const auto& listener : it->second) {
                listener->handle(event);
            }
        }
    }

    // Subscribe a callback to an event of type `EventType`
    template<typename EventType>
    void subscribe(std::function<void(const EventType&)> callback) {
        auto& listenersForType = listeners[typeid(EventType)];
        listenersForType.emplace_back(std::make_unique<ConcreteListener<EventType>>(std::move(callback)));
    }

    // Unsubscribe all callbacks for a specific event type
    template<typename EventType>
    void unsubscribeAll() {
        listeners.erase(typeid(EventType));
    }

    // Clear all event listeners
    void clear() {
        listeners.clear();
    }

    // Check if there are listeners for a specific event type
    template<typename EventType>
    bool hasListeners() const {
        auto it = listeners.find(typeid(EventType));
        return it != listeners.end() && !it->second.empty();
    }

private:
    // Interface for a generic listener
    struct IListener {
        virtual ~IListener() = default;
        virtual void handle(const Event& event) const = 0;
    };

    // Concrete listener implementation for a specific event type
    template<typename EventType>
    struct ConcreteListener : IListener {
        explicit ConcreteListener(std::function<void(const EventType&)> cb)
            : callback(std::move(cb)) {}

        void handle(const Event& event) const override {
            try {
                callback(static_cast<const EventType&>(event));
            } catch (const std::bad_cast&) {
                throw std::runtime_error("Event type mismatch during handling.");
            }
        }

    private:
        std::function<void(const EventType&)> callback;
    };

    // Mapping of event types to their associated listeners
    std::unordered_map<std::type_index, std::vector<std::unique_ptr<IListener>>> listeners;
};
