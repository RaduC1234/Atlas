#pragma once
#include <entt/entt.hpp>

#include "Components.hpp"
#include "utils/Uuid.hpp"

using Registry = entt::registry;
using Actor = entt::entity;

class Actors {
public:

    static uint32_t getActorId(Actor actor);

    // basic
    static Actor createStaticProp(Registry& registry, TransformComponent transform, RenderComponent render);
    static Actor createPawn(Registry& registry, TransformComponent transform, RenderComponent render, PawnComponent pawn);

    // ui
    static Actor createTextbox(Registry& registry, TransformComponent transform, RenderComponent render, TextboxComponent textBox);
    static Actor createButton(Registry& registry, TransformComponent transform, RenderComponent render, ButtonComponent button);
    static void mapToStaticProps(Registry& registry, const std::vector<std::vector<int>> &map);

};


enum class RegistryOperation {
    CREATE_ENTITY,
    DESTROY_ENTITY,
    ADD_COMPONENT,
    REMOVE_COMPONENT
};

struct RegistryModification {
    RegistryOperation operation;
    Actor actor;
    std::string componentType;
    std::vector<JsonData> data;
};
