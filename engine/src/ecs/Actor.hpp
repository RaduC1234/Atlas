#pragma once
#include "Registry.hpp"
#include "ECSException.hpp"
#include <cstdint>
#include <nlohmann/json.hpp> // Need to resolve the problem with the vcpkg
//Need to complete the type registry for now it's ok completing it next day or next life
// Forward declarations , you know what im done for these week
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
    friend class Registry;
};

// Template implementations
template<typename T>
bool Actor::hasComponent() const {
    return registry && registry->getComponent<T>(*this) != nullptr;
}

template<typename T>
T* Actor::getComponent() {
    return registry ? registry->getComponent<T>(*this) : nullptr;
}

template<typename T, typename... Args>
T& Actor::addComponent(Args&&... args) {
    if (!registry) throw ECSException("Actor has no registry");
    return registry->addComponent<T>(*this, std::forward<Args>(args)...);
}

template<typename T>
void Actor::removeComponent() {
    if (registry) registry->removeComponent<T>(*this);
}

// Serialization implementations
inline Json Actor::toJson() const {
    Json json;
    json["id"] = id;
    return json;
}

inline void Actor::fromJson(const Json& json) {
    id = json["id"];
}