#include "Lobby.hpp"
#include "map/MapGenerator.hpp"

Lobby::Lobby() {
}

Lobby::Lobby(Lobby &&other) noexcept
    : registry(std::move(other.registry)),
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

uint64_t Lobby::nextId() {
    return entId++;
}

void Lobby::start() {

    DIRTY_COMPONENT(TransformComponent);
    DIRTY_COMPONENT(PawnComponent);

    constexpr glm::vec2 tileSize = {100.0f, 100.0f};

    auto map = MapGenerator(50, 50).getMap();

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

            const auto actor = registry.create();
            registry.emplace<NetworkComponent>(actor, nextId(), static_cast<uint32_t>(tileNumber + TILE_CODE));
            registry.emplace<TransformComponent>(actor, glm::vec3(posX, posY, 5.0f), 0.0f, tileSize);

            if (tileNumber == 40 || tileNumber == 63) {
                registry.emplace<RigidbodyComponent>(actor, RigidbodyComponent{true});
            }

        }
    }

    this->started = true;
}

void Lobby::setPlayerInput(uint64_t playerId, const PlayerInput &input) {
    std::lock_guard<std::mutex> lock(inputMutex);
    inputQueue[playerId] = input;
}

void Lobby::update(float deltaTime) {
    auto view = registry.view<TransformComponent, NetworkComponent>();

    // Extract the latest player inputs
    std::unordered_map<uint64_t, PlayerInput> latestInputs;
    {
        std::lock_guard<std::mutex> lock(inputMutex);
        latestInputs = std::move(inputQueue);
        inputQueue.clear();
    }
    // Process movement
    for (const auto entity : view) {
        auto &transform = view.get<TransformComponent>(entity);
        const auto &network = view.get<NetworkComponent>(entity);

        if (auto pawn = registry.try_get<PawnComponent>(entity)) {
            pawn->moveForward = false;
            pawn->moveBackwards = false;
            pawn->moveLeft = false;
            pawn->moveRight = false;

            // Find the latest input for this player
            auto it = latestInputs.find(pawn->playerId);
            if (it != latestInputs.end()) {
                const auto &input = it->second;

                pawn->moveForward = input.moveForward;
                pawn->moveBackwards = input.moveBackwards;
                pawn->moveLeft = input.moveLeft;
                pawn->moveRight = input.moveRight;

                if (input.moveForward) transform.position.y += this->baseSpeed * deltaTime;
                if (input.moveBackwards)
                    transform.position.y -= this->baseSpeed * deltaTime;
                if (input.moveLeft) transform.position.x -= this->baseSpeed * deltaTime;
                if (input.moveRight) transform.position.x += this->baseSpeed * deltaTime;

                transform.rotation = input.aimRotation;

                network.dirtyFlag = true;
            }
        }
    }


    nlohmann::json gameState;
    gameState["entities"] = nlohmann::json::array();

    try {
        for (auto entity : view) {
            const auto &network = view.get<NetworkComponent>(entity);

            if (!network.dirtyFlag) {
                continue;
            }
            network.dirtyFlag = false;

            const auto &transform = registry.get<TransformComponent>(entity);

            nlohmann::json entityJson;

            entityJson["id"] = static_cast<int>(entity);
            entityJson["networkId"] = network.networkId;
            entityJson["tile-code"] = static_cast<int>(network.tileCode);

            entityJson["TransformComponent"] = {
                {"position", {transform.position.x, transform.position.y, transform.position.z}},
                {"rotation", transform.rotation},
                {"scale", {transform.scale.x, transform.scale.y}}
            };

            if (auto pawn = registry.try_get<PawnComponent>(entity)) {
                entityJson["PawnComponent"] = {
                    {"playerId", pawn->playerId},
                    {"moveForward", pawn->moveForward},
                    {"moveBackwards", pawn->moveBackwards},
                    {"moveLeft", pawn->moveLeft},
                    {"moveRight", pawn->moveRight},
                    {"aimRotation", pawn->aimRotation},
                    {"isShooting", pawn->isShooting}
                };
            }

            if (auto rigidbody = registry.try_get<RigidbodyComponent>(entity)) {
                entityJson["RigidbodyComponent"] = {
                    {"isSolid", rigidbody->isSolid}
                };
            }

            gameState["entities"].push_back(entityJson);
        }

        // broadcast the game state to all connected clients
        {
            std::lock_guard<std::mutex> lock(playersMutex);
            for (const auto &conn : playerConnections | std::views::values) {
                if (conn) {
                    conn->send_text(gameState.dump());
                }
            }
        }
    } catch (const std::exception &e) {
        AT_ERROR("Error serializing or sending game state: ", e.what());
    }
}

void Lobby::markDirty(entt::registry &registry, entt::entity entity) {
    if (auto network = registry.try_get<NetworkComponent>(entity)) {
        network->dirtyFlag = true;
    }
}

