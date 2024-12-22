#pragma once

#include <imgui.h>
#include <random>
#include <sstream>
#include <iomanip>
#include <unordered_map>

#include "renderer/Camera.hpp"
#include "renderer/RenderManager.hpp"
#include "system/NetworkSystem.hpp"
#include "system/PawnSystem.hpp"
#include "system/RenderSystem.hpp"
#include "map/MapState.hpp"
#include "renderer/MapRenderer.hpp"
#include "network/ClientNetworkService.hpp"

class LevelScene : public Scene {
public:
    // Static utility method to pad tile numbers with zeros
    static std::string getPaddedTileNumber(int tileNum, int padLength = 4) {
        std::ostringstream oss;
        oss << std::setw(padLength) << std::setfill('0') << tileNum;
        return oss.str();
    }

    void onCreate() override {
        loadResources();
        currentMap = MapState();

        if (!isDebugMode) {
            waitForServerMap();
        } else {
            createDemoMap();
        }

        isDebugMode = true;

        loadAllTiles();
        loadPawnTextures();

        // Calculate center of map
        float centerX = (MapState::MAP_WIDTH * MapState::TILE_SIZE) / 2.0f;
        float centerY = (MapState::MAP_HEIGHT * MapState::TILE_SIZE) / 2.0f;

        //=============================Initialize Test Pawn================================
        // Create test pawn at top-left corner
        Actors::createPawn(
            registry,
            {glm::vec3(200.0f, 200.0f, 0.0f), 0.0f, {100.0f, 100.0f}},
            {"front1", RenderComponent::defaultTexCoords(), {1.0f, 1.0f, 1.0f, 1.0f}},
            PawnComponent()
        );
    }

    void onStart() override {
        // Initialize any game state here
    }

    void onUpdate(float deltaTime) override {
        // Center camera on player position
        auto view = registry.view<TransformComponent, PawnComponent>();
        for (auto entity : view) {
            auto& transform = registry.get<TransformComponent>(entity);
            camera.setPosition({transform.position.x, transform.position.y});
            break;  // Only for first player
        }

        ImGui::Begin("Debug Window");
        ImGuiIO &io = ImGui::GetIO();
        ImGui::Text("FPS: %.1f", io.Framerate);
        ImGui::Text("Mouse Screen: (%.1f, %.1f)", Mouse::getX(), Mouse::getY());
        auto coords = camera.screenToWorld({Mouse::getX(), Mouse::getY()});
        ImGui::Text("Mouse World: (%.1f, %.1f)", coords.x, coords.y);

        // Add camera zoom control
        static float zoom = 0.5f;
        if (ImGui::SliderFloat("Camera Zoom", &zoom, 0.1f, 2.0f)) {
            camera.setZoom(zoom);
        }

        // Add toggle for wireframe mode
        static bool wireFrame = false;
        ImGui::Checkbox("Wireframe", &wireFrame);
        if (wireFrame) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        if (isDebugMode) {
            handleDebugMode();
        }

        pawnSystem.update(deltaTime, registry, localPlayerId, camera);
        renderSystem.update(deltaTime, registry);
        networkSystem.update(deltaTime, registry);

        ImGui::End();
    }

    void onRender(int screenWidth, int screenHeight) override {
        // Maintain aspect ratio and proper camera bounds
        const float baseWidth = 1920.0f;
        const float baseHeight = 1080.0f;

        float aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
        float desiredAspectRatio = baseWidth / baseHeight;

        camera.applyViewport(screenWidth, screenHeight, desiredAspectRatio);
        RenderManager::flush(screenWidth, screenHeight, camera);
    }

    void onDestroy() override {
        ResourceManager::clearAll();
    }

private:
    void loadResources() {
        ResourceManager::load<Font>("minecraft", "assets/fonts/Minecraft.ttf");
        ResourceManager::loadFromDirectory<Texture>("assets/textures/tiles", nullptr, ".png");
        ResourceManager::loadFromDirectory<Texture>("assets/textures/pawns", nullptr, ".png");
    }

    void loadAllTiles() {
        // Mapping of TileType to specific tile numbers
        std::unordered_map<TileType, int> tileMappings = {
            {TileType::Empty,             0},           // tile_0000.png
            {TileType::Grass,             49},           // tile_0001.png
            {TileType::Bush,              2},           // tile_0002.png
            {TileType::DestructibleWall,  37},          // Use tile_0049.png for destructible wall
            {TileType::IndestructibleWall, 37},         // Use tile_0050.png for indestructible wall
            {TileType::HealthPickup,      51},          // Use tile_0051.png for health pickup
            {TileType::SpeedBoost,        52},          // Use tile_0052.png for speed boost
            {TileType::HiddenBomb,        53}           // Use tile_0053.png for hidden bomb
        };

        // Load tiles for each type
        for (const auto& [tileType, tileNumber] : tileMappings) {
            // Create unique tile ID that matches the file name
            std::string tileID = "tile_" + getPaddedTileNumber(tileNumber);

            // Construct file path for the tile
            std::string filePath = "assets/textures/old/tiles/new/tile_" + getPaddedTileNumber(tileNumber) + ".png";

            // Load the tile texture
            ResourceManager::load<Texture>(tileID, filePath);
        }
    }

    // Optional method to add individual tiles dynamically
    void addAdditionalTile(int tileNum) {
        // Create unique tile ID that matches the file name
        std::string tileID = "tile_" + getPaddedTileNumber(tileNum);

        // Construct file path for the tile
        std::string filePath = "assets/textures/old/tiles/new/tile_" + getPaddedTileNumber(tileNum) + ".png";

        // Load the tile texture
        ResourceManager::load<Texture>(tileID, filePath);
    }

    void loadPawnTextures() {
        ResourceManager::load<Texture>("back1", "assets/textures/pawns/back1.png");
        ResourceManager::load<Texture>("back2", "assets/textures/pawns/back2.png");
        ResourceManager::load<Texture>("front1", "assets/textures/pawns/front1.png");
        ResourceManager::load<Texture>("front2", "assets/textures/pawns/front1.png");
        ResourceManager::load<Texture>("left1", "assets/textures/pawns/left1.png");
        ResourceManager::load<Texture>("left2", "assets/textures/pawns/left1.png");
        ResourceManager::load<Texture>("right1", "assets/textures/pawns/right1.png");
        ResourceManager::load<Texture>("right2", "assets/textures/pawns/right1.png");
    }

    void createDemoMap() {
        // Store player position if it exists
        Actor playerEntity = entt::null;
        glm::vec3 playerPos;

        auto view = registry.view<PawnComponent>();
        for (auto entity : view) {
            playerEntity = entity;
            auto& transform = registry.get<TransformComponent>(entity);
            playerPos = transform.position;
            break;
        }

        // Create base map
        for (uint32_t y = 0; y < MapState::MAP_HEIGHT; y++) {
            for (uint32_t x = 0; x < MapState::MAP_WIDTH; x++) {
                auto& tile = currentMap.at(x, y);
                if (x == 0 || x == MapState::MAP_WIDTH-1 ||
                    y == 0 || y == MapState::MAP_HEIGHT-1) {
                    tile.type = TileType::IndestructibleWall;
                } else {
                    tile.type = ((x + y) % 2 == 0) ? TileType::Empty : TileType::Grass;
                }
                tile.updateProperties();
            }
        }

        addTestWalls();
        registry.clear();
        MapRenderer::renderMap(currentMap, registry);

        // Restore player if it existed
        if (playerEntity != entt::null) {
            Actors::createPawn(
                registry,
                {playerPos, 0.0f, {100.0f, 100.0f}},
                {"front1", RenderComponent::defaultTexCoords(), {1.0f, 1.0f, 1.0f, 1.0f}},
                PawnComponent()
            );
        }
    }

    void addTestWalls() {
        std::vector<std::pair<uint32_t, uint32_t>> wallPositions = {
            {10, 10}, {10, 11}, {11, 10}, {11, 11},
            {40, 40}, {40, 41}, {41, 40}, {41, 41}
        };

        for (const auto& [x, y] : wallPositions) {
            auto& tile = currentMap.at(x, y);
            tile.type = TileType::DestructibleWall;
            tile.updateProperties();
        }
    }

    void waitForServerMap() {
        // TODO: Will be implemented when server is ready
    }

    void handleDebugMode() {
        ImGui::Begin("Map Debug");

        if (ImGui::Button("Regenerate Map")) {
            createDemoMap();
        }

        static TileType selectedTile = TileType::Empty;
        ImGui::Separator();
        ImGui::Text("Tile Type:");

        const char* tileTypes[] = {
            "Empty", "Grass", "Bush", "Destructible Wall",
            "Indestructible Wall", "Health Pickup", "Speed Boost", "Hidden Bomb"
        };

        for (int i = 0; i < 8; i++) {
            if (ImGui::RadioButton(tileTypes[i], static_cast<int>(selectedTile) == i)) {
                selectedTile = static_cast<TileType>(i);
            }
        }

        if (Mouse::isButtonPressed(Mouse::ButtonLeft)) {
            auto worldPos = camera.screenToWorld({Mouse::getX(), Mouse::getY()});
            int tileX = static_cast<int>(worldPos.x / MapState::TILE_SIZE);
            int tileY = static_cast<int>(worldPos.y / MapState::TILE_SIZE);

            if (currentMap.isValidPosition(tileX, tileY)) {
                auto& tile = currentMap.at(tileX, tileY);
                if (tile.type != selectedTile) {
                    tile.type = selectedTile;
                    tile.updateProperties();
                    MapRenderer::refreshTile(currentMap, tileX, tileY, registry);
                }
            }
        }

        ImGui::End();
    }

    Camera camera{{0.0f, 0.0f}, 0.5f};  // Start with 0.5 zoom for better visibility
    Registry registry;
    MapState currentMap;
    uint32_t localPlayerId{0};
    bool isDebugMode{false};

    PawnSystem pawnSystem;
    RenderSystem renderSystem;
    NetworkSystem networkSystem;
};