#pragma once
#include <glm/vec2.hpp>

#include "Atlas.hpp"
#include "AtlasButton.hpp"

class UIManager{
public:
    UIManager(Registry& registry) : registry(registry){}

    AtlasButton createButton(float x, float y, float width, float height, std::function<void()> onClick) {
        return AtlasButton(registry,x,y,width,height,onClick);
    }

    void handleInput(glm::vec2 position, bool isMouseClicked) {
        auto view = registry.view<Position, Size, Button>();
        for (auto entity : view) {
            auto& pos = view.get<Position>(entity);
            auto& size = view.get<Size>(entity);
            auto& button = view.get<Button>(entity);

            // Check if mouse position is within button bounds
            if (position.x >= pos.x && position.x <= pos.x + size.width &&
                position.y >= pos.y && position.y <= pos.y + size.height) {
                if (isMouseClicked && button.onClick) {
                    button.onClick();
                }
                }
        }
    }


private:
    Registry& registry;
};
