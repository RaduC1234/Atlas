#pragma once
#include <functional>
#include "glm/glm.hpp"

class Camera;
struct TransformComponent;
struct RenderComponent;

enum class UIState {
    Normal,
    Hovered,
    Pressed,
    Disabled
};