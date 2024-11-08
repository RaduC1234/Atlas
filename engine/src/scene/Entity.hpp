#pragma once

#include "entt/entt.hpp"

using Registry = entt::registry;
using Actor = entt::entity;

struct TransformComponent {
    glm::vec2 position;
};

struct SpriteComponent {
    Sprite sprite;
};
