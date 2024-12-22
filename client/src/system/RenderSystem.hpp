#pragma once

#include <Atlas.hpp>
#include "renderer/RenderManager.hpp"

class RenderSystem {
public:
    // Define render layers - lower numbers render first
    enum class RenderLayer {
        BACKGROUND = 0,
        TILES = 1,
        ITEMS = 2,
        PLAYERS = 3,
        UI = 4
    };

    void update(double deltaTime, Registry &registry) {
        // Group entities by layer
        std::map<RenderLayer, std::vector<entt::entity>> layeredEntities;

        auto view = registry.view<TransformComponent, RenderComponent>();
        for (auto entity : view) {
            RenderLayer layer = RenderLayer::TILES; // Default layer

            // Determine layer based on entity type
            if (registry.any_of<PawnComponent>(entity)) {
                layer = RenderLayer::PLAYERS;
            }
            else if (registry.any_of<ButtonComponent, TextboxComponent>(entity)) {
                layer = RenderLayer::UI;
            }
            else {
                // For tiles and other entities, use z-index to determine relative order within layer
                auto& transform = view.get<TransformComponent>(entity);
                if (transform.position.z <= 1.0f) {
                    layer = RenderLayer::BACKGROUND;
                }
                else if (transform.position.z <= 2.0f) {
                    layer = RenderLayer::TILES;
                }
                else {
                    layer = RenderLayer::ITEMS;
                }
            }

            layeredEntities[layer].push_back(entity);
        }

        // Sort entities within each layer by z-index
        for (auto& [layer, entities] : layeredEntities) {
            std::sort(entities.begin(), entities.end(),
                [&registry](const entt::entity& a, const entt::entity& b) {
                    const auto& transformA = registry.get<TransformComponent>(a);
                    const auto& transformB = registry.get<TransformComponent>(b);
                    return transformA.position.z < transformB.position.z;
                });
        }

        // Render each layer in order
        for (int layer = 0; layer <= static_cast<int>(RenderLayer::UI); ++layer) {
            auto currentLayer = static_cast<RenderLayer>(layer);
            if (layeredEntities.find(currentLayer) != layeredEntities.end()) {
                for (auto entity : layeredEntities[currentLayer]) {
                    renderEntity(registry, entity);
                }
            }
        }

        // Handle additional UI rendering (text for buttons, etc)
        if (layeredEntities.find(RenderLayer::UI) != layeredEntities.end()) {
            renderUIElements(registry);
        }
    }

private:
    void renderEntity(Registry &registry, entt::entity entity) {
        const auto& transform = registry.get<TransformComponent>(entity);
        const auto& render = registry.get<RenderComponent>(entity);

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
                break;

            default:
                break;
        }
    }

    void renderUIElements(Registry &registry) {
        auto buttonView = registry.view<TransformComponent, ButtonComponent>();
        for (const auto entity : buttonView) {
            const auto& transform = buttonView.get<TransformComponent>(entity);
            const auto& button = buttonView.get<ButtonComponent>(entity);

            if (!button.text.empty()) {
                glm::vec3 textPosition = transform.position;
                textPosition.x -= transform.scale.x * 0.025f;
                textPosition.y -= transform.scale.y * 0.22f;
                textPosition.z = transform.position.z - 0.1f;

                glm::vec4 currentTextColor = button.isPressed ? button.pressedTextColor :
                                      button.isHovered ? button.hoverTextColor :
                                      button.normalTextColor;


                RenderManager::drawText(
                    textPosition,
                    button.text,
                    ResourceManager::get<Font>(button.font),
                    transform.scale.y / 20,
                    currentTextColor,
                    true
                );
            }
        }

        auto textboxView = registry.view<TransformComponent, TextboxComponent>();
        for (const auto entity : textboxView) {
            const auto& transform = textboxView.get<TransformComponent>(entity);
            const auto& textbox = textboxView.get<TextboxComponent>(entity);

            glm::vec3 textPosition = transform.position;
            textPosition.x -= transform.scale.x * 0.42f;
            textPosition.y -= transform.scale.y * 0.15f;
            textPosition.z = transform.position.z - 0.1f;

            RenderManager::drawText(
                textPosition,
                textbox.isPassword ? std::string(textbox.text.length(), '*') : textbox.text,
                ResourceManager::get<Font>(textbox.font),
                transform.scale.y / 20,
                textbox.textColor,
                false
            );
        }
    }
};