#include "Lobby.hpp"

#include "map/MapGenerator.hpp"

Lobby::Lobby() {
    constexpr glm::vec2 tileSize = {100.0f, 100.0f};

    const std::vector<std::vector<int>> map = {
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1}
    };

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
            std::cout << tileNumber;

            const auto actor = registry.create();
            registry.emplace<NetworkComponent>(actor, nextId(), static_cast<uint32_t>(0));
            registry.emplace<TransformComponent>(actor, glm::vec3(posX, posY, 5.0f), 0.0f, tileSize);
        }
    }
}

Lobby::Lobby(Lobby &&other) noexcept: registry(std::move(other.registry)),
                                      players(std::move(other.players)),
                                      entId(other.entId) {
}

Lobby &Lobby::operator=(Lobby &&other) noexcept {
    if (this != &other) {
        std::lock_guard<std::mutex> lock(other.registryMutex);
        registry = std::move(other.registry);
        players = std::move(other.players);
        entId = other.entId;
    }
    return *this;
}

bool Lobby::containsPlayer(uint64_t playerId) {
    return std::ranges::find(players, playerId) != players.end();
}

void Lobby::serializeRegistry(nlohmann::json &outJson) {
    std::lock_guard<std::mutex> lock(registryMutex);
    outJson["entities"] = nlohmann::json::array();

    auto view = registry.view<NetworkComponent, TransformComponent>();
    for (auto entity: view) {
        auto &network = view.get<NetworkComponent>(entity);
        auto &transform = registry.get<TransformComponent>(entity);

        nlohmann::json entityJson;
        entityJson["id"] = static_cast<int>(entity);
        entityJson["networkId"] = network.networkId;
        entityJson["tile-code"] = static_cast<int>(network.tileCode);

        entityJson["TransformComponent"] = {
            {"position", {transform.position.x, transform.position.y, transform.position.z}},
            {"rotation", transform.rotation},
            {"scale", {transform.scale.x, transform.scale.y}}
        };

        if (registry.any_of<PawnComponent>(entity)) {
            auto &pawn = registry.get<PawnComponent>(entity);
            entityJson["PawnComponent"] = {
                {"playerId", pawn.playerId},
                {"moveForward", pawn.moveForward},
                {"moveBackwards", pawn.moveBackwards},
                {"moveLeft", pawn.moveLeft},
                {"moveRight", pawn.moveRight},
                {"aimRotation", pawn.aimRotation}
            };
        }

        if (registry.any_of<RigidbodyComponent>(entity)) {
            auto &rigidbody = registry.get<RigidbodyComponent>(entity);
            entityJson["RigidbodyComponent"] = {
                {"isSolid", rigidbody.isSolid}
            };
        }

        outJson["entities"].push_back(entityJson);
    }
}

uint64_t Lobby::nextId() {
    return entId++;
}
