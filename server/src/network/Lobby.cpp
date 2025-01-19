#include "Lobby.hpp"
#include "map/MapGenerator.hpp"

Lobby::Lobby() {
}

Lobby::Lobby(Lobby &&other) noexcept
    : registry(std::move(other.registry)),
      players(std::move(other.players)),
      playerSpawnPoints(std::move(other.playerSpawnPoints)),
      entId(other.entId) {
}

Lobby &Lobby::operator=(Lobby &&other) noexcept {
    if (this != &other) {
        std::lock_guard<std::mutex> lock(other.registryMutex);
        registry = std::move(other.registry);
        players = std::move(other.players);
        playerSpawnPoints = std::move(other.playerSpawnPoints);
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

bool Lobby::isPositionInsideFireball(const glm::vec3& spawnPosition) {
    auto fireballView = registry.view<FireballComponent, TransformComponent>();

    for (auto otherEntity : fireballView) {
        const auto &otherTransform = fireballView.get<TransformComponent>(otherEntity);

        float spawnLeft = spawnPosition.x - 50.0f;
        float spawnRight = spawnPosition.x + 50.0f;
        float spawnTop = spawnPosition.y + 50.0f;
        float spawnBottom = spawnPosition.y - 50.0f;

        float otherLeft = otherTransform.position.x - 50.0f;
        float otherRight = otherTransform.position.x + 50.0f;
        float otherTop = otherTransform.position.y + 50.0f;
        float otherBottom = otherTransform.position.y - 50.0f;

        if (spawnRight > otherLeft && spawnLeft < otherRight &&
            spawnTop > otherBottom && spawnBottom < otherTop) {
            return true;  // Collision detected
        }
    }
    return false;  // No collision
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

            if (tileNumber == 40 || tileNumber == 63 || tileNumber == 64) {
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

    for (const auto entity : view) {
        if (auto pawn = registry.try_get<PawnComponent>(entity)) {
            uint64_t playerId = pawn->playerId;

            // Ensure the player has lives initialized
            if (playerLives.find(playerId) == playerLives.end()) {
                playerLives[playerId] = 3; // Set initial lives

            }

            // Log the current lives for debugging

        }
    }

    std::unordered_map<uint64_t, PlayerInput> latestInputs;
    {
        std::lock_guard<std::mutex> lock(inputMutex);
        latestInputs = std::move(inputQueue);
        inputQueue.clear();
    }

    // Collect entities to destroy outside the main loop
    std::vector<entt::entity> entitiesToDestroy;

    for (const auto entity : view) {
        auto &transform = view.get<TransformComponent>(entity);
        const auto &network = view.get<NetworkComponent>(entity);

        if (auto pawn = registry.try_get<PawnComponent>(entity)) {
            uint64_t playerId = pawn->playerId;
            if (playerLives[playerId] < 1) {
                // Log the player status for debugging

                continue; // Skip input processing and movement for this player
            }
            glm::vec3 originalPos = transform.position;

            pawn->moveForward = false;
            pawn->moveBackwards = false;
            pawn->moveLeft = false;
            pawn->moveRight = false;
            pawn->aimRotation = 0.0f;
            pawn->isShooting = false;

            auto it = latestInputs.find(pawn->playerId);
            if (it != latestInputs.end()) {
                const auto &input = it->second;

                pawn->moveForward = input.moveForward;
                pawn->moveBackwards = input.moveBackwards;
                pawn->moveLeft = input.moveLeft;
                pawn->moveRight = input.moveRight;
                pawn->aimRotation = input.aimRotation;
                pawn->isShooting = input.isShooting;

                float currentTime = Time::now().toSeconds();
                if (pawn->isShooting && canPlayerShoot(pawn->playerId, currentTime)) {
                    updatePlayerLastShotTime(pawn->playerId, currentTime);

                    glm::vec3 fireballDirection = glm::vec3(glm::cos(input.aimRotation), -glm::sin(input.aimRotation), 3.0f);
                    fireballDirection = normalize(fireballDirection);
                    glm::vec3 spawnOffset = glm::vec3(fireballDirection.x, fireballDirection.y, 3.0f) * 400.0f;
                    glm::vec3 spawnPosition = transform.position;
                    spawnPosition.x += spawnOffset.x;
                    spawnPosition.y += spawnOffset.y;

                    if (!isPositionInsideFireball(spawnPosition)) {
                        auto fireballEntity = registry.create();
                        AT_INFO("Creating fireball for player {}", pawn->playerId);

                        registry.emplace<NetworkComponent>(fireballEntity, nextId(), TILE_CODE+110, transform.position, true);
                        registry.emplace<FireballComponent>(fireballEntity, spawnPosition, fireballDirection, 800.0f, pawn->playerId);
                        registry.emplace<TransformComponent>(fireballEntity, spawnPosition, input.aimRotation, glm::vec2(100.0f,100.0f));
                    } else {
                        AT_INFO("Fireball spawn blocked: Position inside another fireball");
                    }
                }

                if (input.moveLeft) transform.position.x -= this->baseSpeed * deltaTime;
                if (input.moveRight) transform.position.x += this->baseSpeed * deltaTime;

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

                if (input.moveForward) transform.position.y += this->baseSpeed * deltaTime;
                if (input.moveBackwards) transform.position.y -= this->baseSpeed * deltaTime;

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

                transform.rotation = 0.0f;
                network.dirtyFlag = true;
            }
        }

        // Handle fireball updates and collisions
        if (auto fireball = registry.try_get<FireballComponent>(entity)) {
            if (std::ranges::find(entitiesToDestroy, entity) != entitiesToDestroy.end()) {
                continue;
            }

            glm::vec3 newPosition = fireball->position;
            newPosition.x += fireball->direction.x * fireball->speed * deltaTime;
            newPosition.y += fireball->direction.y * fireball->speed * deltaTime;
            newPosition.z = 3.0f;

            fireball->position = newPosition;
            transform.position = newPosition;
            auto &network = view.get<NetworkComponent>(entity);
            network.dirtyFlag = true;

            bool collisionDetected = false;

            // Check collision with walls
            auto wallView = registry.view<RigidbodyComponent, TransformComponent, NetworkComponent>();
            for (auto wall : wallView) {
                const auto &wallTransform = wallView.get<TransformComponent>(wall);
                const auto &rigidbody = wallView.get<RigidbodyComponent>(wall);
                const auto &wallNetwork = wallView.get<NetworkComponent>(wall);

                if (!rigidbody.isSolid || entity == wall) continue;

                float fireballLeft = newPosition.x - 50.0f;
                float fireballRight = newPosition.x + 50.0f;
                float fireballTop = newPosition.y + 50.0f;
                float fireballBottom = newPosition.y - 50.0f;

                float wallLeft = wallTransform.position.x - (wallTransform.scale.x * 0.5f);
                float wallRight = wallTransform.position.x + (wallTransform.scale.x * 0.5f);
                float wallTop = wallTransform.position.y + (wallTransform.scale.y * 0.5f);
                float wallBottom = wallTransform.position.y - (wallTransform.scale.y * 0.5f);

                if (fireballRight > wallLeft && fireballLeft < wallRight &&
                    fireballTop > wallBottom && fireballBottom < wallTop) {
                    if (wallNetwork.tileCode == TILE_CODE + 63) {
                        // Replace destructible wall with a path tile
                        registry.emplace_or_replace<NetworkComponent>(wall, wallNetwork.networkId, TILE_CODE + 48, wallTransform.position, true);
                        registry.emplace_or_replace<TransformComponent>(wall, wallTransform.position, wallTransform.rotation, wallTransform.scale);
                        registry.emplace_or_replace<RigidbodyComponent>(wall, RigidbodyComponent{false});
                        AT_INFO("Destructible wall replaced with path tile.");
                    } else if (wallNetwork.tileCode == TILE_CODE + 64) {
                        // Bomb explosion logic
                        float explosionRadius = 500.0f;
                        auto blastView = registry.view<TransformComponent, NetworkComponent, RigidbodyComponent>();
                        for (auto target : blastView) {
                            const auto &targetTransform = blastView.get<TransformComponent>(target);
                            auto &targetNetwork = blastView.get<NetworkComponent>(target);

                            float distance = glm::distance(targetTransform.position, wallTransform.position);
                            if (distance <= explosionRadius) {
                                if (targetNetwork.tileCode == TILE_CODE + 63 || // Destructible wall
                                    targetNetwork.tileCode == TILE_CODE + 64 || // Bomb
                                    targetNetwork.tileCode == TILE_CODE + 40) { // Other wall types
                                    registry.emplace_or_replace<NetworkComponent>(target, targetNetwork.networkId, TILE_CODE + 48, targetTransform.position, true);
                                    registry.emplace_or_replace<TransformComponent>(target, targetTransform.position, targetTransform.rotation, targetTransform.scale);
                                    registry.emplace_or_replace<RigidbodyComponent>(target, RigidbodyComponent{false});
                                    AT_INFO("Object within blast radius replaced with path.");
                                } else if (auto playerPawn = registry.try_get<PawnComponent>(target)) {
                                    // Respawn player to original position
                                    auto spawnPointIt = playerSpawnPoints.find(playerPawn->playerId);
                                    if (spawnPointIt != playerSpawnPoints.end()) {
                                        auto &playerTransform = registry.get<TransformComponent>(target);
                                        playerTransform.position = spawnPointIt->second.position;
                                        if (auto *playerNetwork = registry.try_get<NetworkComponent>(target)) {
                                            playerNetwork->dirtyFlag = true;
                                        }
                                        AT_INFO("Player {} hit by bomb! Respawning at original position.", playerPawn->playerId);
                                    }
                                }
                            }
                        }

                        // Replace the bomb itself with a path tile
                        registry.emplace_or_replace<NetworkComponent>(wall, wallNetwork.networkId, TILE_CODE + 48, wallTransform.position, true);
                        registry.emplace_or_replace<TransformComponent>(wall, wallTransform.position, wallTransform.rotation, wallTransform.scale);
                        registry.emplace_or_replace<RigidbodyComponent>(wall, RigidbodyComponent{false});
                        AT_INFO("Bomb exploded and replaced with path.");
                    }

                    collisionDetected = true;
                    entitiesToDestroy.push_back(entity);
                    break;
                }
            }

            // Check collision with other fireballs
            if (!collisionDetected) {
                auto fireballView = registry.view<FireballComponent, TransformComponent>();
                for (auto otherEntity : fireballView) {
                    if (entity == otherEntity ||
                        std::ranges::find(entitiesToDestroy, otherEntity) != entitiesToDestroy.end()) {
                        continue;
                    }

                    const auto &otherTransform = fireballView.get<TransformComponent>(otherEntity);

                    float fireballLeft = newPosition.x - 50.0f;
                    float fireballRight = newPosition.x + 50.0f;
                    float fireballTop = newPosition.y + 50.0f;
                    float fireballBottom = newPosition.y - 50.0f;

                    float otherLeft = otherTransform.position.x - 50.0f;
                    float otherRight = otherTransform.position.x + 50.0f;
                    float otherTop = otherTransform.position.y + 50.0f;
                    float otherBottom = otherTransform.position.y - 50.0f;

                    if (fireballRight > otherLeft && fireballLeft < otherRight &&
                        fireballTop > otherBottom && fireballBottom < otherTop) {
                        collisionDetected = true;
                        entitiesToDestroy.push_back(entity);
                        entitiesToDestroy.push_back(otherEntity);
                        break;
                    }
                }
            }

            // Check collision with players
            if (!collisionDetected) {
                auto playerView = registry.view<PawnComponent, TransformComponent>();
                for (auto playerEntity : playerView) {
                    auto &playerTransform = registry.get<TransformComponent>(playerEntity);
                    auto &pawn = registry.get<PawnComponent>(playerEntity);

                    float fireballLeft = newPosition.x - 50.0f;
                    float fireballRight = newPosition.x + 50.0f;
                    float fireballTop = newPosition.y + 50.0f;
                    float fireballBottom = newPosition.y - 50.0f;

                    float playerLeft = playerTransform.position.x - (playerTransform.scale.x * 0.4f);
                    float playerRight = playerTransform.position.x + (playerTransform.scale.x * 0.4f);
                    float playerTop = playerTransform.position.y + (playerTransform.scale.y * 0.4f);
                    float playerBottom = playerTransform.position.y - (playerTransform.scale.y * 0.4f);

                    if (fireballRight > playerLeft && fireballLeft < playerRight &&
                        fireballTop > playerBottom && fireballBottom < playerTop) {
                        // Find the spawn point for this player
                        auto spawnPointIt = playerSpawnPoints.find(pawn.playerId);
                        if (spawnPointIt != playerSpawnPoints.end()) {
                            // Set player position back to spawn point
                            playerTransform.position = spawnPointIt->second.position;
                            if (auto* network = registry.try_get<NetworkComponent>(playerEntity)) {
                                network->dirtyFlag = true;
                            }

                            if (playerLives.find(pawn.playerId) != playerLives.end()) {
                                if ( playerLives[pawn.playerId]>0)
                                playerLives[pawn.playerId]--;


                            }
                            AT_INFO("Player {} hit by fireball! Respawning at original position.", pawn.playerId, playerLives[pawn.playerId]);
                        }

                        collisionDetected = true;
                        entitiesToDestroy.push_back(entity); // Destroy the fireball
                        break;
                    }
                }
            }

            if (collisionDetected &&
                std::ranges::find(entitiesToDestroy, entity) == entitiesToDestroy.end()) {
                entitiesToDestroy.push_back(entity);
            }
        }
    }

    // Remove duplicate entities from destruction list
    auto last = std::ranges::unique(entitiesToDestroy).begin();
    entitiesToDestroy.erase(last, entitiesToDestroy.end());

    // Destroy entities after updating
    for (const auto destroyEntity : entitiesToDestroy) {
        if (registry.valid(destroyEntity)) {
            markDeleted(registry, destroyEntity);
            registry.destroy(destroyEntity);
        }
    }

    nlohmann::json gameState;
    gameState["entities"] = nlohmann::json::array();

    gameState["deletedEntities"] = this->deletedEntities;
    this->deletedEntities.clear();

    try {
        for (auto entity : view) {
            const auto &network = view.get<NetworkComponent>(entity);

            if (!network.dirtyFlag && this->synced) {
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

        this->synced = true;

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

void Lobby::markDeleted(entt::registry &registry, entt::entity entity) {
    if (auto network = registry.try_get<NetworkComponent>(entity)) {
        this->deletedEntities.push_back(network->networkId);
    }
}