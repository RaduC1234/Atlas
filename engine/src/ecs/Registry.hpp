#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <queue>
#include <bitset>
#include <iostream>
#include <algorithm>
#include "Actor.hpp"

constexpr std::size_t MAX_COMPONENTS = 64;

class ComponentTypeRegistry {
public:
    template<typename Component>
    static std::size_t getId() {
        static const std::size_t id = nextId++;
        return id;
    }

private:
    static inline std::size_t nextId = 0;
};

class ComponentMask {
public:
    void set(std::size_t id) {
        mask.set(id);
    }

    void reset(std::size_t id) {
        mask.reset(id);
    }

    [[nodiscard]] bool matches(const ComponentMask& other) const {
        return (mask & other.mask) == other.mask;
    }

private:
    std::bitset<MAX_COMPONENTS> mask;
};

class Registry {
public:
    template<typename... Components>
    friend class View;

    Registry() = default;

    Actor createEntity() {
        Actor::IdType id = (freeIds.empty()) ? nextEntityId++ : freeIds.front();
        if (!freeIds.empty()) freeIds.pop();

        Actor actor(id, this);
        entities.push_back(actor);
        entityMasks[id] = ComponentMask();
        std::cout << "Created entity: " << id << std::endl;
        return actor;
    }

    void destroyEntity(Actor entity) {
        if (!entity.isValid() || entity.getRegistry() != this) {
            throw std::runtime_error("Invalid entity or entity does not belong to this registry.");
        }

        for (auto& [type, map] : components) {
            map->remove(entity.getId());
        }

        entityMasks.erase(entity.getId());
        freeIds.push(entity.getId());
        std::erase_if(entities, [entity](const Actor& e) { return e == entity; });

        std::cout << "Destroyed entity: " << entity.getId() << std::endl;
    }

    template<typename T>
    T* getComponent(Actor entity) {
        auto& map = getComponentMap<T>();
        return map.get(entity.getId());
    }

    template<typename T, typename... Args>
    T& addComponent(Actor entity, Args&&... args) {
        auto& map = getComponentMap<T>();
        auto& component = map.add(entity.getId(), T(std::forward<Args>(args)...));
        entityMasks[entity.getId()].set(ComponentTypeRegistry::getId<T>());
        return component;
    }

    template<typename T>
    void removeComponent(Actor entity) {
        auto& map = getComponentMap<T>();
        map.remove(entity.getId());
        entityMasks[entity.getId()].reset(ComponentTypeRegistry::getId<T>());
    }

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

        Iterator begin() { return Iterator(registry, 0, componentMask); }
        Iterator end() { return Iterator(registry, registry.entities.size(), componentMask); }

    private:
        Registry& registry;
        ComponentMask componentMask;
    };

    template<typename... Components>
    View<Components...> view() {
        return View<Components...>(*this);
    }

private:
    struct IComponentMap {
        virtual ~IComponentMap() = default;
        virtual void remove(Actor::IdType id) = 0;
        virtual void* get(Actor::IdType id) = 0;
    };

    template<typename T>
    struct ComponentMap : IComponentMap {
        std::unordered_map<Actor::IdType, T> map;

        T* get(Actor::IdType id) override {
            auto it = map.find(id);
            return it != map.end() ? &it->second : nullptr;
        }

        T& add(Actor::IdType id, T&& component) {
            return map.insert_or_assign(id, std::move(component)).first->second;
        }

        void remove(Actor::IdType id) override {
            map.erase(id);
        }
    };

    template<typename T>
    ComponentMap<T>& getComponentMap() {
        auto type = std::type_index(typeid(T));
        if (!components.contains(type)) {
            components[type] = std::make_unique<ComponentMap<T>>();
        }
        return static_cast<ComponentMap<T>&>(*components[type]);
    }

    std::vector<Actor> entities;
    std::unordered_map<std::type_index, std::unique_ptr<IComponentMap>> components;
    std::unordered_map<Actor::IdType, ComponentMask> entityMasks;
    std::queue<Actor::IdType> freeIds;
    Actor::IdType nextEntityId = 1;
};
