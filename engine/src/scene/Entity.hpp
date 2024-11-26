#pragma once

#include <entt/entt.hpp>

#include "Components.hpp"


using Registry = entt::registry;
using Actor = entt::entity;

class Actors {
    static Actor createStaticProp(Registry& registry, TransformComponent transform, RenderComponent render);
    static Actor createPawn(Registry& registry, TransformComponent transform, RenderComponent render, PawnComponent pawn);
    // static Actor createLight(Registry& registry, LightComponent light);
};
