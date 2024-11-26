#include "Entity.hpp"

Actor Actors::createStaticProp(Registry &registry, TransformComponent transform, RenderComponent render) {
    auto actor = registry.create();
    registry.emplace<TransformComponent>(actor, transform);
    registry.emplace<RenderComponent>(actor, render);

    return actor;
}

Actor Actors::createPawn(Registry &registry, TransformComponent transform, RenderComponent render, PawnComponent pawn) {
    auto actor = createStaticProp(registry, transform, render);
    registry.emplace<PawnComponent>(actor, pawn);

    return actor;
}
