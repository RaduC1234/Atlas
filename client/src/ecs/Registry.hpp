#pragma once
#include "core/Core.hpp"
#include "Actor.hpp"

class Registry {
public:
    Actor createEntity() {
        Actor::IdType id = nextEntityId++;
        entities.emplace_back(id);
        return Actor(id);
    }

    void destroyEntity(Actor entity) {
        for (auto& [type, map] : components) {
            map->remove(entity.getId());
        }
        entities.erase(std::remove_if(entities.begin(), entities.end(),
            [entity](const Actor& e) { return e == entity; }), entities.end());
    }

    template<typename Component>
    void addComponent(Actor entity, Component component) {
        auto& map = getComponentMap<Component>();
        map[entity.getId()] = std::make_shared<Component>(component);
    }

    template<typename Component>
    Component* getComponent(Actor entity) {
        auto& map = getComponentMap<Component>();
        auto it = map.find(entity.getId());
        return it != map.end() ? it->second.get() : nullptr;
    }

    template<typename Component>
    void removeComponent(Actor entity) {
        auto& map = getComponentMap<Component>();
        map.erase(entity.getId());
    }

    template<typename Component>
    std::unordered_map<Actor::IdType, std::shared_ptr<Component>>& getComponentMap() {
        auto type = std::type_index(typeid(Component));
        if (components.find(type) == components.end()) {
            components[type] = std::make_unique<ComponentMap<Component>>();
        }
        return components[type]->as<Component>();
    }

private:
    struct IComponentMap {
        virtual ~IComponentMap() = default;
    };

    template<typename Component>
    struct ComponentMap : IComponentMap {
        std::unordered_map<Actor::IdType, std::shared_ptr<Component>> map;
        std::unordered_map<Actor::IdType, std::shared_ptr<Component>>& as() {
            return map;
        }
    };

    std::unordered_map<std::type_index, std::unique_ptr<IComponentMap>> components;
    std::vector<Actor> entities;
    Actor::IdType nextEntityId = 1;
};
