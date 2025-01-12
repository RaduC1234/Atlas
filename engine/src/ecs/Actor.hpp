#pragma once

#include <cstdint>
#include <nlohmann/json.hpp> // Ensure this is properly installed via vcpkg or any other package manager
#include "Registry.hpp"
#include "ECSException.hpp"

class Registry;
using Json = nlohmann::json;

class Actor {
public:
    using IdType = std::uint32_t;

    Actor() = default;
    explicit Actor(IdType id, Registry* registry = nullptr)
        : id(id), registry(registry) {}

    [[nodiscard]] IdType getId() const { return id; }
    [[nodiscard]] bool isValid() const { return id != 0; }
    [[nodiscard]] Registry* getRegistry() const { return registry; }

    template<typename T>
    [[nodiscard]] bool hasComponent() const;

    template<typename T>
    T* getComponent();

    template<typename T, typename... Args>
    T& addComponent(Args&&... args);

    template<typename T>
    void removeComponent();

    bool operator==(const Actor& other) const { return id == other.id; }
    bool operator!=(const Actor& other) const { return !(*this == other); }

    // Serialization
    [[nodiscard]] Json toJson() const;
    void fromJson(const Json& json);

private:
    IdType id = 0;
    Registry* registry = nullptr;

    friend class Registry; // Allows the Registry class to access private members
};

// Template implementations
template<typename T>
bool Actor::hasComponent() const {
    return registry && registry->template getComponent<T>(*this) != nullptr;
}

template<typename T>
T* Actor::getComponent() {
    return registry ? registry->template getComponent<T>(*this) : nullptr;
}

template<typename T, typename... Args>
T& Actor::addComponent(Args&&... args) {
    if (!registry) throw ECSException("Actor has no registry");
    return registry->template addComponent<T>(*this, std::forward<Args>(args)...);
}

template<typename T>
void Actor::removeComponent() {
    if (registry) registry->template removeComponent<T>(*this);
}

// Serialization implementations
inline Json Actor::toJson() const {
    Json json;
    json["id"] = id;
    return json;
}

inline void Actor::fromJson(const Json& json) {
    if (json.contains("id")) {
        id = json.at("id").get<IdType>();
    }
}
