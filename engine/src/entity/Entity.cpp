#include "Entity.hpp"

#include <imgui.h>

uint32_t Actors::getActorId(Actor actor) {
    return entt::to_integral(actor);
}

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

Actor Actors::createTextbox(Registry &registry, TransformComponent transform, RenderComponent render, TextboxComponent textBox) {
    auto actor = createStaticProp(registry, transform, render);
    registry.emplace<TextboxComponent>(actor, textBox);

    return actor;
}

Actor Actors::createButton(Registry &registry, TransformComponent transform, RenderComponent render, ButtonComponent button) {
    auto actor = createStaticProp(registry, transform, render);
    registry.emplace<ButtonComponent>(actor, button);

    return actor;
}

void Actors::mapToStaticProps(Registry &registry, const std::vector<std::vector<int>> &map){
    constexpr glm::vec2 tileSize = {100.0f, 100.0f};

    int width = map[0].size();
    int height = map.size();

    float offsetX = -(width * tileSize.x) / 2.0f + tileSize.x / 2.0f;
    float offsetY = -(height * tileSize.y) / 2.0f + tileSize.y / 2.0f;

    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            int tileNumber = map[row][col];

            if (tileNumber < 0) {
                continue;
            }

            float posX = col * tileSize.x + offsetX;
            float posY = row * tileSize.y + offsetY;

            std::string tileID = std::string("tile") +
                     (tileNumber < 10 ? "00" : (tileNumber < 100 ? "0" : "")) +
                     std::to_string(tileNumber);


            TransformComponent transform(glm::vec3(posX, posY, 2.0f), 0.0f, {tileSize.x, tileSize.y});
            RenderComponent render(tileID, RenderComponent::defaultTexCoords(), {1.0f, 1.0f, 1.0f, 1.0f});

            createStaticProp(registry, transform, render);
        }
    }
}
