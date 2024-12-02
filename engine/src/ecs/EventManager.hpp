#pragma once

#include <iostream>
#include <functional>
#include <unordered_map>
#include <typeindex>
#include <vector>
#include <memory>

class Event {
public:
    virtual ~Event() = default;
};

class EventManager {
public:
    template<typename EventType>
    void emit(const EventType& event) {
        for (const auto& listener : listeners[typeid(EventType)]) {
            listener->handle(event);
        }
    }

    template<typename EventType>
    void subscribe(std::function<void(const EventType&)> callback) {
        listeners[typeid(EventType)].emplace_back(
            std::make_unique<ConcreteListener<EventType>>(std::move(callback)));
    }

private:
    struct IListener {
        virtual ~IListener() = default;
        virtual void handle(const Event& event) = 0;
    };

    template<typename EventType>
    struct ConcreteListener : IListener {
        explicit ConcreteListener(std::function<void(const EventType&)> cb)
            : callback(std::move(cb)) {}
        void handle(const Event& event) override {
            callback(static_cast<const EventType&>(event));
        }

    private:
        std::function<void(const EventType&)> callback;
    };

    std::unordered_map<std::type_index, std::vector<std::unique_ptr<IListener>>> listeners;
};
