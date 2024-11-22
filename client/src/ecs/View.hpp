#pragma once

#include "Registry.hpp"

// View class for efficient iteration
template<typename... Components>
class View {
public:
    explicit View(Registry& registry) : registry(registry) {
        (componentMask.set(ComponentTypeRegistry::getId<Components>()), ...);
    }

    class Iterator {
    public:
        Iterator(Registry& registry, std::size_t index, const ComponentMask& mask)
            : registry(registry), currentIndex(index), componentMask(mask) {
            skipToValidEntity();
        }

        Iterator& operator++() {
            ++currentIndex;
            skipToValidEntity();
            return *this;
        }

        Actor operator*() const {
            return registry.entities[currentIndex];
        }

        bool operator!=(const Iterator& other) const {
            return currentIndex != other.currentIndex;
        }

    private:
        Registry& registry;
        std::size_t currentIndex;
        ComponentMask componentMask;

        void skipToValidEntity() {
            while (currentIndex < registry.entities.size() &&
                   !registry.entityMasks[registry.entities[currentIndex].getId()].matches(componentMask)) {
                ++currentIndex;
                   }
        }

    };

    [[nodiscard]] Iterator begin() {
        return Iterator(registry, 0, componentMask);
    }

    [[nodiscard]] Iterator end() {
        return Iterator(registry, registry.entities.size(), componentMask);
    }

private:
    Registry& registry;
    ComponentMask componentMask;
};
