#pragma once

#include <Atlas.hpp>
#include "scene/Scene.hpp"
#include "system/RenderSystem.hpp"
#include "system/UISystem.hpp"
#include "map/MapState.hpp"
#include "network/ClientNetworkService.hpp"
//#include "renderer/MapRenderer.hpp"

class MapScene : public Scene {
public:
    void onCreate() override {
        // Initialize resources or assets for the scene (if needed)
    }

    void onStart() override {
        // Fetch the map data from the server
        camera.setZoom(0.5f);  // Adjust the zoom level as needed
        camera.setPosition({0.0f, 0.0f});  // Set camera position if necessary
        try {
            MapState mapState = ClientNetworkService::getMapData();  // Fetch the map data from server
            //MapRenderer::renderMap(mapState,registry);
            renderMapText(mapState);
        } catch (const std::exception &e) {
            AT_ERROR("Failed to fetch or render map: {0}", e.what());  // Log error if map fetching fails
        }
    }

    void onUpdate(float deltaTime) override {
        renderSystem.update(registry);
    }

    void onRender(int screenWidth, int screenHeight) override {
        // Optionally, you can handle camera adjustments and additional rendering logic here
        RenderManager::flush(screenWidth, screenHeight, camera);
    }

    void onDestroy() override {
        // Cleanup resources when the scene is destroyed
    }

private:
    RenderSystem renderSystem;
    UISystem uiSystem;
    Camera camera{{0.0f, 0.0f}, 0.5f};  // Define camera (if needed)

    // Render the map as text (characters)
    void renderMapText(const MapState& mapState) {
        std::string mapText = "";
        for (uint32_t y = 0; y < MapState::MAP_HEIGHT; ++y) {
            for (uint32_t x = 0; x < MapState::MAP_WIDTH; ++x) {
                const MapTile& tile = mapState.at(x, y);
                mapText += getTileCharacter(tile.type);  // Get the character representation of the tile
            }
            mapText += "\n";  // Move to the next line after each row
        }

        AT_INFO("Map Layout:\n{0}", mapText);  // Log the map layout as text in the console
    }

    // Function to get the character representation for a given tile type
    std::string getTileCharacter(TileType type) {
        switch (type) {
            case TileType::Path: return "."; //Path tile
            case TileType::Grass: return "_";  // Grass tile
            case TileType::Bush: return "@";   // Bush tile
            case TileType::DestructibleWall: return "#";  // Destructible wall tile
            case TileType::IndestructibleWall: return "$";  // Indestructible wall tile
            case TileType::HealthPickup: return "H";  // Health Pickup tile
            case TileType::SpeedBoost: return "S";   // Speed Boost tile
            case TileType::HiddenBomb: return "X";  // Hidden Bomb tile
            default: return " ";  // Default for unknown or empty tiles
        }
    }
};
