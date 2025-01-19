#include "RenderManager.hpp"

void RenderManager::init(uint32_t maxBatchSize) {
    renderer = Renderer(maxBatchSize);
}

void RenderManager::shutdown() {
    renderer = Renderer(); // default constructor creates nothing
}

void RenderManager::drawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, float rotation, const glm::vec4 &color, const Sprite &sprite, bool centered, int32_t specialProperties) {
    renderer.drawPrimitive(position, size, rotation, Shape::QUAD, color, sprite.texture, sprite.texCoords, centered, specialProperties);
}

void RenderManager::drawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color, const Sprite &sprite, bool centered, int32_t specialProperties) {
    renderer.drawPrimitive(position, size, 0.0f, Shape::QUAD, color, sprite.texture, sprite.texCoords, centered, specialProperties);
}

void RenderManager::flush(uint32_t screenWidth, uint32_t screenHeight, Camera &camera) {
    renderer.flush(screenWidth, screenHeight, camera);
}

void RenderManager::drawText(const glm::vec3 &position, const std::string &text, const Ref<Font> &font, float size, const glm::vec4 &color, bool centered) {
    renderer.drawText(position, size * 0.25f, color, *font.get(), text, centered);
}

Renderer RenderManager::renderer = Renderer();
