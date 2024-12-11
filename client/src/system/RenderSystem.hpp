#pragma once

#include <Atlas.hpp>
#include "renderer/RenderManager.hpp"

class RenderSystem {
public:
    void update(double deltaTime, Registry &registry) {
        // Handle all render components (quads, text, and textboxes)
        auto view = registry.view<TransformComponent, RenderComponent>();

        for (const auto &entity: view) {
            const auto &transform = view.get<TransformComponent>(entity);
            const auto &render = view.get<RenderComponent>(entity);

            switch (render.shape) {
                case 0: // Quad
                    RenderManager::drawRotatedQuad(
                        transform.position,
                        transform.scale,
                        transform.rotation,
                        render.color,
                        ResourceManager::get<Texture>(render.textureKey),
                        render.isCentered,
                        render.renderFlags
                    );
                    break;

                case 2: // Text
                    RenderManager::drawText(
                        transform.position,
                        render.text,
                        ResourceManager::get<Font>(render.fontKey),
                        transform.scale.x,
                        render.color,
                        render.isCentered
                    );

                default:
                    break;
            }
        }

        auto buttonView = registry.view<TransformComponent, RenderComponent, ButtonComponent>();
        for (const auto &entity: buttonView) {
            const auto &transform = buttonView.get<TransformComponent>(entity);
            const auto &render = buttonView.get<RenderComponent>(entity);
            const auto &button = buttonView.get<ButtonComponent>(entity);

            // Render button text
            if (!button.text.empty()) {
                glm::vec3 textPosition = transform.position;
                textPosition.x -= transform.scale.x * 0.025f;
                textPosition.y -= transform.scale.y * 0.22f;

                RenderManager::drawText(
                    glm::vec3(textPosition.x, textPosition.y, transform.position.z - 1),
                    button.text,
                    ResourceManager::get<Font>(button.font),
                    transform.scale.y / 20,
                    button.textColor,
                    true // Center the text
                );
            }
        }

        auto textboxView = registry.view<TransformComponent, TextboxComponent>();

        for (const auto &entity: textboxView) {
            const auto &transform = textboxView.get<TransformComponent>(entity);
            const auto &textbox = textboxView.get<TextboxComponent>(entity);

            glm::vec3 textPosition = transform.position;
            textPosition.x -= transform.scale.x * 0.42f;
            textPosition.y -= transform.scale.y * 0.15f;

            RenderManager::drawText(
                glm::vec3(textPosition.x, textPosition.y, transform.position.z - 1),
                textbox.text,
                ResourceManager::get<Font>(textbox.font),
                transform.scale.y / 20,
                textbox.textColor,
                false
            );
        }
    }
};
