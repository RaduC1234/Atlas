#pragma once

#include <glm/glm.hpp>
#include "component/Components.hpp"
#include "resource/ResourceManager.hpp"
#include "scene/Entity.hpp"

// Animation state structure to keep PawnComponent clean
struct AnimationState {
    float moveSpeed = 200.0f;
    float animationSpeed = 0.3f;
    float idleAnimationSpeed = 0.5f;
    bool isMoving = false;
    float animationTimer = 0.0f;
    float idleTimer = 0.0f;
    bool useAltFrame = false;
    bool idleUseAltFrame = false;
    std::string currentDirection = "characterS"; // Default to front-facing
};

class PawnSystem {
public:
    PawnSystem() {
        animationStates.reserve(100); // Reserve space for efficiency
    }

    void update(double deltaTime, Registry& registry, Camera& camera) {
        auto view = registry.view<TransformComponent, PawnComponent, RenderComponent>();

        for(const auto& entity : view) {
            auto& transform = view.get<TransformComponent>(entity);
            auto& pawn = view.get<PawnComponent>(entity);
            auto& render = view.get<RenderComponent>(entity);

            // Create animation state if it doesn't exist
            if (animationStates.find(pawn.serverId) == animationStates.end()) {
                animationStates[pawn.serverId] = AnimationState();
            }

            auto& anim = animationStates[pawn.serverId];

            updateMovement(deltaTime, transform, render, anim);
            updateAnimation(deltaTime, render, anim);

            // Center camera on character
            camera.setPosition(transform.position);
        }
    }

private:
    void updateMovement(double deltaTime, TransformComponent& transform, RenderComponent& render, AnimationState& anim) {
        anim.isMoving = false;

        if (Keyboard::isKeyPressed(Keyboard::A)) {
            anim.isMoving = true;
            transform.position.x -= deltaTime * anim.moveSpeed;
            updateSprite(render, anim, "characterA");
        }
        else if (Keyboard::isKeyPressed(Keyboard::D)) {
            anim.isMoving = true;
            transform.position.x += deltaTime * anim.moveSpeed;
            updateSprite(render, anim, "characterD");
        }
        else if (Keyboard::isKeyPressed(Keyboard::S)) {
            anim.isMoving = true;
            transform.position.y -= deltaTime * anim.moveSpeed;
            updateSprite(render, anim, "characterS");
        }
        else if (Keyboard::isKeyPressed(Keyboard::W)) {
            anim.isMoving = true;
            transform.position.y += deltaTime * anim.moveSpeed;
            updateSprite(render, anim, "characterW");
        }
    }

    void updateAnimation(double deltaTime, RenderComponent& render, AnimationState& anim) {
        if (anim.isMoving) {
            anim.animationTimer += deltaTime;
            if (anim.animationTimer >= anim.animationSpeed) {
                anim.animationTimer = 0.0f;
                anim.useAltFrame = !anim.useAltFrame;
                updateSprite(render, anim, anim.currentDirection);
            }
        }
        else {
            anim.idleTimer += deltaTime;
            if (anim.idleTimer >= anim.idleAnimationSpeed) {
                anim.idleTimer = 0.0f;
                anim.idleUseAltFrame = !anim.idleUseAltFrame;
                updateSprite(render, anim, anim.currentDirection);
            }
        }
    }

    void updateSprite(RenderComponent& render, AnimationState& anim, const std::string& baseKey) {
        anim.currentDirection = baseKey;
        bool useAlt = anim.isMoving ? anim.useAltFrame : anim.idleUseAltFrame;
        render.textureKey = useAlt ? baseKey + "_Alt" : baseKey;
        render.coords = TextureCoords(); // Reset texture coordinates
    }

private:
    std::unordered_map<uint32_t, AnimationState> animationStates;
};