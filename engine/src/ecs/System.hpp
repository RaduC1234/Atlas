#pragma once
#include "Registry.hpp"
#include "Components.hpp"
#include <iostream> // fuck the error I have it in the #include "core/Core.hpp" why its no finding it !!!!
class System {
public:
    virtual ~System() = default;
    virtual void update(Registry& registry, float deltaTime) = 0;
};


class MovementSystem : public System {
public:
    void update(Registry& registry, float deltaTime) override {

        for (auto entity : registry.view<Position, Velocity>()) {
            auto* position = entity.getComponent<Position>();
            auto* velocity = entity.getComponent<Velocity>();
            
            if (position && velocity) {
                position->x += velocity->dx * deltaTime;
                position->y += velocity->dy * deltaTime;
            }
        }
    }
};

// Example Health System using the new View system
class HealthSystem : public System {
public:
    void update(Registry& registry, float /*deltaTime*/) override {
        // Using the new view system
        for (auto entity : registry.view<Health>()) {
            auto* health = entity.getComponent<Health>();
            
            if (health && health->hp <= 0) {
                std::cout << "Entity " << entity.getId() << " is dead.\n";
                // Could emit a death event here using the event system
            }
        }
    }
};