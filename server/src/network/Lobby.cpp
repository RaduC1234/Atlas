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
    DIRTY_COMPONENT(FireballComponent);

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
            // Store the original position before movement
            glm::vec3 originalPos = transform.position;

            pawn->moveForward = false;
            pawn->moveBackwards = false;
            pawn->moveLeft = false;
            pawn->moveRight = false;
            pawn->aimRotation = 0.0f;
            pawn->isShooting = false;

            // Find the latest input for this player
            auto it = latestInputs.find(pawn->playerId);
            if (it != latestInputs.end()) {
                const auto &input = it->second;

                pawn->moveForward = input.moveForward;
                pawn->moveBackwards = input.moveBackwards;
                pawn->moveLeft = input.moveLeft;
                pawn->moveRight = input.moveRight;
                pawn->aimRotation = input.aimRotation;
                pawn->isShooting = input.isShooting;

                if (pawn->isShooting) {
                    auto fireballEntity = registry.create();
                    AT_INFO("Creating fireball for player {}", pawn->playerId);
                    glm::vec3 fireballDirection = glm::vec3(glm::cos(input.aimRotation), glm::sin(input.aimRotation), 0.0f);
                    registry.emplace<NetworkComponent>(fireballEntity,nextId(),TILE_CODE+48,transform.position,true);
                    registry.emplace<FireballComponent>(fireballEntity, transform.position, fireballDirection, 300.0f, pawn->playerId);
                    registry.emplace<TransformComponent>(fireballEntity, transform.position, 0.0f, glm::vec2(100.0f,100.0f));
                }


                // Apply horizontal movement
                if (input.moveLeft) transform.position.x -= this->baseSpeed * deltaTime;
                if (input.moveRight) transform.position.x += this->baseSpeed * deltaTime;

                // Check horizontal collisions
                bool xCollision = false;
                auto view2 = registry.view<RigidbodyComponent, TransformComponent>();

                for (auto wall : view2) {
                    const auto& wallTransform = view2.get<TransformComponent>(wall);
                    const auto& rigidbody = view2.get<RigidbodyComponent>(wall);

                    if (!rigidbody.isSolid || entity == wall) {
                        continue;
                    }

                    float playerLeft = transform.position.x - (transform.scale.x * 0.4f);
                    float playerRight = transform.position.x + (transform.scale.x * 0.4f);
                    float wallLeft = wallTransform.position.x - (wallTransform.scale.x * 0.5f);
                    float wallRight = wallTransform.position.x + (wallTransform.scale.x * 0.5f);
                    float playerTop = transform.position.y + (transform.scale.y * 0.4f);
                    float playerBottom = transform.position.y - (transform.scale.y * 0.4f);
                    float wallTop = wallTransform.position.y + (wallTransform.scale.y * 0.5f);
                    float wallBottom = wallTransform.position.y - (wallTransform.scale.y * 0.5f);

                    if (playerRight > wallLeft && playerLeft < wallRight &&
                        playerTop > wallBottom && playerBottom < wallTop) {
                        xCollision = true;
                        break;
                    }
                }

                if (xCollision) {
                    transform.position.x = originalPos.x;
                }

                // Apply vertical movement
                if (input.moveForward) transform.position.y += this->baseSpeed * deltaTime;
                if (input.moveBackwards) transform.position.y -= this->baseSpeed * deltaTime;

                // Check vertical collisions
                bool yCollision = false;
                for (auto wall : view2) {
                    const auto& wallTransform = view2.get<TransformComponent>(wall);
                    const auto& rigidbody = view2.get<RigidbodyComponent>(wall);

                    if (!rigidbody.isSolid || entity == wall) {
                        continue;
                    }

                    float playerLeft = transform.position.x - (transform.scale.x * 0.4f);
                    float playerRight = transform.position.x + (transform.scale.x * 0.4f);
                    float wallLeft = wallTransform.position.x - (wallTransform.scale.x * 0.5f);
                    float wallRight = wallTransform.position.x + (wallTransform.scale.x * 0.5f);
                    float playerTop = transform.position.y + (transform.scale.y * 0.4f);
                    float playerBottom = transform.position.y - (transform.scale.y * 0.4f);
                    float wallTop = wallTransform.position.y + (wallTransform.scale.y * 0.5f);
                    float wallBottom = wallTransform.position.y - (wallTransform.scale.y * 0.5f);

                    if (playerRight > wallLeft && playerLeft < wallRight &&
                        playerTop > wallBottom && playerBottom < wallTop) {
                        yCollision = true;
                        break;
                    }
                }

                if (yCollision) {
                    transform.position.y = originalPos.y;
                }

                transform.rotation = 0.0f; // do not remove this. it brakes the movement for some reason
                network.dirtyFlag = true;
            }
        }
        if(auto fireball = registry.try_get<FireballComponent>(entity)){
            // Update fireball position based on its direction and speed
            glm::vec3 newPosition = fireball->position + fireball->direction * fireball->speed * deltaTime;

            // Check collision with rigid bodies
            auto rigidbodyView = registry.view<RigidbodyComponent, TransformComponent>();
            bool collisionDetected = false;

            for (auto target : rigidbodyView) {
                const auto &rigidbodyTransform = rigidbodyView.get<TransformComponent>(target);
                const auto &rigidbody = rigidbodyView.get<RigidbodyComponent>(target);

                if (!rigidbody.isSolid || entity == target) {
                    continue;
                }

                // Collision detection
                float fireballLeft = newPosition.x - 50.0f;
                float fireballRight = newPosition.x + 50.0f;
                float fireballTop = newPosition.y + 50.0f;
                float fireballBottom = newPosition.y - 50.0f;
                float wallLeft = rigidbodyTransform.position.x - (rigidbodyTransform.scale.x * 0.5f)-50.0f;
                float wallRight = rigidbodyTransform.position.x + (rigidbodyTransform.scale.x * 0.5f)+50.0f;
                float wallTop = rigidbodyTransform.position.y + (rigidbodyTransform.scale.y * 0.5f)+50.0f;
                float wallBottom = rigidbodyTransform.position.y - (rigidbodyTransform.scale.y * 0.5f)-50.0f;

                if (fireballRight > wallLeft && fireballLeft < wallRight &&
                    fireballTop > wallBottom && fireballBottom < wallTop) {
                    collisionDetected = true;
                    break;
                    }
            }

            if (collisionDetected) {
                // Destroy fireball on collision
                registry.destroy(entity);
            } else {
                // Update fireball position if no collision
                fireball->position = newPosition;
                transform.position = newPosition;
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