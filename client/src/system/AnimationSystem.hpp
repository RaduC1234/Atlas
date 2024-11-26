#pragma once

#include <unordered_map>
#include <memory>
#include "component/Components.hpp"
#include "scene/Entity.hpp"
#include "resource/Animation.hpp"

class AnimationSystem {
public:
    void update(double deltaTime, Registry& registry) {
        auto view = registry.view<AnimationComponent, RenderComponent>();

        for (const auto entity : view) {
            auto& animation = view.get<AnimationComponent>(entity);
            auto& render = view.get<RenderComponent>(entity);

            // Update the animation component's frame
            animation.update(deltaTime);

            // Update the RenderComponent's texture to the current frame
            if (const auto& frame = animation.getCurrentFrame(); !frame.empty()) {
                render.textureKey = frame;
            }
        }
    }

    void validateAllAnimations(Registry& registry) {
        auto view = registry.view<AnimationComponent>();
        for (const auto entity : view) {
            const auto& animation = view.get<AnimationComponent>(entity);
            animation.validateAnimations();
        }
    }
};
