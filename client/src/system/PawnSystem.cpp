#include "PawnSystem.hpp"

void PawnSystem::update(double deltaTime, entt::registry &registry) {
    registry.view<PawnComponent, RenderComponent>().each([&](auto entity, PawnComponent &pawn, RenderComponent &render) {
        if (pawn.moveForward && pawn.moveLeft) {
            render.textureKey = "pawn_back_left01";  // Diagonal: back + left
        } else if (pawn.moveForward && pawn.moveRight) {
            render.textureKey = "pawn_back_right01"; // Diagonal: back + right
        } else if (pawn.moveForward) {
            render.textureKey = "pawn_back01";       // Backward only
        } else if (pawn.moveLeft && pawn.moveBackwards) {
            render.textureKey = "pawn_front_left01"; // Diagonal: front + left
        } else if (pawn.moveRight && pawn.moveBackwards) {
            render.textureKey = "pawn_front_right01"; // Diagonal: front + right
        } else if (pawn.moveLeft) {
            render.textureKey = "pawn_left01";       // Left only
        } else if (pawn.moveRight) {
            render.textureKey = "pawn_right01";      // Right only
        } else if (pawn.moveBackwards) {
            render.textureKey = "pawn_front01";      // Front only
        } else {
            render.textureKey = "pawn_front01";       // No movement
        }
    });
}

