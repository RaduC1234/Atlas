#include "RenderManager.hpp"

void RenderManager::init(uint32_t maxBatchSize) {
    renderer = Renderer(maxBatchSize, {1.0f, 1.0f, 1.0f, 1.0f});
}

void RenderManager::shutdown() {
    renderer = Renderer(); // default constructor creates nothing
}

void RenderManager::drawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, float rotation, const glm::vec4 &color, Sprite sprite) {
    renderer.drawRotatedQuad(position, size, rotation, color, sprite);
}

void RenderManager::drawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color, Sprite sprite) {
    renderer.drawQuad(position, size, color, sprite);
}

void RenderManager::flush(uint32_t screenWidth, uint32_t screenHeight, Camera &camera) {
    renderer.flush(screenWidth, screenHeight, camera);
}

void RenderManager::drawText(const glm::vec3 &position, const std::string &text, const Ref<Font> &font, float size, const glm::vec4 &color) {
    renderer.drawText(position, size * 0.25f, color, *font.get(), text);
}

Renderer RenderManager::renderer = Renderer();

