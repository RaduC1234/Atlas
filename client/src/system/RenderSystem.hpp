#pragma once

#include "component/Components.hpp"
#include "renderer/RenderManager.hpp"
#include "resource/ResourceManager.hpp"
#include "scene/Entity.hpp"

class RenderSystem {
public:
    void update(double deltaTime, Registry& registry) {
        auto view = registry.view<TransformComponent, RenderComponent>();

        for(const auto& actor : view) {
            auto&[position, rotation, scale] = view.get<TransformComponent>(actor);
            auto&[textureKey, coords, color, shape, text, fontKey] = view.get<RenderComponent>(actor);

            switch (shape) {
                case QUAD:
                    RenderManager::drawRotatedQuad(position, scale, rotation, color, ResourceManager::get<Texture>(textureKey));
                    break;

                case TEXT:
                    RenderManager::drawText(position, text, ResourceManager::get<Font>(fontKey), scale.x, color);
                    break;

                case CIRCLE: // ignore for now
                default: ;
            }
        }
    }
};
